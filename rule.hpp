#ifndef RULE_HPP
#define RULE_HPP

#include <algorithm>
#include <array>
#include <cassert>
#include <cctype>
#include <functional>
#include <iterator>
#include <limits>
#include <optional>
#include <string_view>
#include <tuple>
#include <vector>

// The rule class is intended to enable the (reasonably) straightforward
// conversion of ABNF grammars to C++ code.
// -
class rule {
public:
  using acceptor_container = std::vector<
    std::tuple<std::function<void (std::string_view)>, std::string_view>>;

private:
  rule (std::optional<std::string_view> string,
        acceptor_container const& acceptors)
      : tail_{string}, acceptors_{acceptors} {}
  rule () = default;

public:
  explicit rule (std::string_view string) : tail_{string} {}
  rule (rule const& rhs) = default;
  rule (rule&& rhs) noexcept = default;
  ~rule () noexcept = default;

  rule& operator= (rule const& rhs) = default;
  rule& operator= (rule&& rhs) noexcept = default;

  bool done () {
    if (!tail_ || !tail_->empty ()) {
      return false;
    }
    std::for_each (std::begin (acceptors_), std::end (acceptors_),
                   [] (acceptor_container::value_type const& a) {
                     std::get<0> (a) (std::get<1> (a));
                   });
    return true;
  }

  template <typename MatchFunction, typename AcceptFunction,
            typename = std::enable_if_t<
              std::is_invocable_v<MatchFunction, rule&&> &&
              std::is_invocable_v<AcceptFunction, std::string_view>>>
  rule concat (MatchFunction match, AcceptFunction accept) {
    return concat_impl (match, accept, false);
  }

  static void accept_nop (std::string_view) { /* do nothing */
  }

  template <
    typename MatchFunction,
    typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule&&>>>
  rule concat (MatchFunction match) {
    return concat_impl (match, &rule::accept_nop, false);
  }

  template <typename MatchFunction, typename AcceptFunction,
            typename = std::enable_if_t<
              std::is_invocable_v<MatchFunction, rule&&> &&
              std::is_invocable_v<AcceptFunction, std::string_view>>>
  rule optional (MatchFunction match, AcceptFunction accept);

  template <
    typename MatchFunction,
    typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule&&>>>
  rule optional (MatchFunction match);

  // Variable Repetition:  *Rule
  //
  // The operator "*" preceding an element indicates repetition.  The full form
  // is:
  //
  //   <a>*<b>element
  //
  // where <a> and <b> are optional decimal values, indicating at least <a> and
  // at most <b> occurrences of the element.
  //
  // Default values are 0 and infinity so that *<element> allows any number,
  // including zero; 1*<element> requires at least one; 3*3<element> allows
  // exactly 3 and 1*2<element> allows one or two.
  template <
    typename MatchFunction,
    typename = std::enable_if_t<std::is_invocable_v<MatchFunction, rule&&>>>
  rule star (MatchFunction match, unsigned min = 0,
             unsigned max = std::numeric_limits<unsigned>::max ());

  rule alternative () { return {}; }
  template <typename MatchFunction, typename... Rest>
  rule alternative (MatchFunction match, Rest&&... rest);

  constexpr std::optional<std::string_view> tail () const { return tail_; }

  using matched_result =
    std::optional<std::tuple<std::string_view, acceptor_container>>;

  matched_result matched (char const* name, rule const& in) const;

  template <typename Predicate>
  matched_result single_char (Predicate pred) const;
  matched_result single_char (char const c) const {
    return single_char ([c2 = std::tolower (static_cast<int> (c))] (char d) {
      return c2 == std::tolower (static_cast<int> (d));
    });
  }

private:
  template <typename MatchFunction, typename AcceptFunction>
  rule concat_impl (MatchFunction match, AcceptFunction accept, bool optional);

  rule join_rule (matched_result::value_type const& m) {
    auto const& [head, acc] = m;
    acceptors_.insert (acceptors_.end (), acc.begin (), acc.end ());
    return {tail_->substr (head.length ()), acceptors_};
  }

  template <typename Function>
  constexpr bool is_nop (Function f) const noexcept;

  acceptor_container acceptors_;
  std::optional<std::string_view> tail_;
};

// star
// ~~~~
template <typename MatchFunction, typename>
rule rule::star (MatchFunction const match, unsigned const min,
                 unsigned const max) {
  if (!tail_) {
    return *this;
  }
  auto length = std::string_view::size_type{0};
  std::string_view str = *tail_;
  auto count = 0U;
  for (;;) {
    matched_result const m = match (rule{str});
    if (!m) {
      break;  // No match so no more repetitions.
    }
    ++count;
    if (count > max) {
      break;
    }
    auto const l = std::get<std::string_view> (*m).length ();
    str.remove_prefix (l);
    length += l;

    auto const& a = std::get<acceptor_container> (*m);
    acceptors_.insert (acceptors_.end (), a.begin (), a.end ());
  }
  if (count < min) {
    return {};
  }

  return {tail_->substr (length), acceptors_};
}

// alternative
// ~~~~~~~~~~~
template <typename MatchFunction, typename... Rest>
rule rule::alternative (MatchFunction match, Rest&&... rest) {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  if (matched_result const m = match (rule{*tail_})) {
    return join_rule (*m);
  }
  // This didn't match, so try the next one.
  return this->alternative (std::forward<Rest> (rest)...);
}

template <typename Function>
constexpr bool rule::is_nop (Function f) const noexcept {
  if constexpr (std::is_pointer_v<Function>) {
    if (f == rule::accept_nop) {
      return true;
    }
  }
  return false;
}

// optional
// ~~~~~~~~
template <typename MatchFunction, typename AcceptFunction, typename>
rule rule::optional (MatchFunction match, AcceptFunction accept) {
  if (!tail_) {
    return *this;  // If matching previously failed, yield failure.
  }
  rule res = rule{*tail_}.concat_impl (match, accept, true);
  if (!res.tail_) {
    return *this;  // The rule failed, so carry on as if nothing happened.
  }
  acceptors_.insert (acceptors_.end (), res.acceptors_.begin (),
                     res.acceptors_.end ());
  return {res.tail_, acceptors_};
}

template <typename MatchFunction, typename>
rule rule::optional (MatchFunction match) {
  return this->optional (match, &rule::accept_nop);
}

// concat impl
// ~~~~~~~~~~~
template <typename MatchFunction, typename AcceptFunction>
rule rule::concat_impl (MatchFunction match, AcceptFunction accept,
                        bool optional) {
  if (!tail_) {
    // If matching has already failed, then pass that condition down the chain.
    return *this;
  }
  if (matched_result m = match (rule{*tail_})) {
    if (!is_nop (accept)) {
      std::get<acceptor_container> (*m).emplace_back (
        accept, std::get<std::string_view> (*m));
    }
    return join_rule (*m);
  }
  if (optional) {
    return *this;
  }
  return {};  // Matching failed: yield nothing or failure.
}

// single char
// ~~~~~~~~~~~
template <typename Predicate>
auto rule::single_char (Predicate const pred) const -> matched_result {
  auto const sv = this->tail ();
  if (sv && !sv->empty () && pred (sv->front ())) {
    return std::make_tuple (sv->substr (0, 1), acceptor_container{});
  }
  return {};
}

inline auto single_char (char const first) {
  return [=] (rule const& r) { return r.single_char (first); };
}
inline auto char_range (char const first, char const last) {
  return [f = std::tolower (static_cast<int> (first)),
          l = std::tolower (static_cast<int> (last))] (rule const& r) {
    return r.single_char ([=] (char const c) {
      auto const cl = std::tolower (static_cast<int> (c));
      return cl >= f && cl <= l;
    });
  };
}

inline auto alpha (rule const& r) {
  return r.single_char (
    [] (char const c) { return std::isalpha (static_cast<int> (c)); });
}
inline auto digit (rule const& r) {
  return r.single_char (
    [] (char const c) { return std::isdigit (static_cast<int> (c)); });
}
inline auto hexdig (rule const& r) {
  return r.single_char (
    [] (char const c) { return std::isxdigit (static_cast<int> (c)); });
}

inline auto commercial_at (rule const& r) {
  return r.single_char ('@');
}
inline auto colon (rule const& r) {
  return r.single_char (':');
}
inline auto hash (rule const& r) {
  return r.single_char ('#');
}
inline auto plus (rule const& r) {
  return r.single_char ('+');
}
inline auto minus (rule const& r) {
  return r.single_char ('-');
}
inline auto solidus (rule const& r) {
  return r.single_char ('/');
}
inline auto question_mark (rule const& r) {
  return r.single_char ('?');
}
inline auto full_stop (rule const& r) {
  return r.single_char ('.');
}
inline auto left_square_bracket (rule const& r) {
  return r.single_char ('[');
}
inline auto right_square_bracket (rule const& r) {
  return r.single_char (']');
}

#endif  // RULE_HPP
