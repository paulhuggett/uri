//===- include/uri/parts.hpp ------------------------------*- mode: C++ -*-===//
//*                            *
//*  _ __   __ _ _ __| |_ ___  *
//* | '_ \ / _` | '__| __/ __| *
//* | |_) | (_| | |  | |_\__ \ *
//* | .__/ \__,_|_|   \__|___/ *
//* |_|                        *
//===----------------------------------------------------------------------===//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//
#ifndef URI_PARTS_HPP
#define URI_PARTS_HPP

#include "uri/icubaby.hpp"
#include "uri/pctencode.hpp"
#include "uri/punycode.hpp"
#include "uri/uri.hpp"

namespace uri {

enum class parts_field { scheme, userinfo, host, port, path, query, fragment };

constexpr pctencode_set pctencode_set_from_parts_field (
  parts_field const field) noexcept {
  switch (field) {
  case parts_field::userinfo: return pctencode_set::userinfo;
  case parts_field::path: return pctencode_set::path;
  case parts_field::query: return pctencode_set::query;
  case parts_field::fragment: return pctencode_set::fragment;
  case parts_field::host:
  case parts_field::port:
  case parts_field::scheme:
  default: return pctencode_set::none;
  }
}

namespace details {

template <typename T>
class ro_sink_container {
public:
  using value_type = T;

  // NOLINTNEXTLINE(hicpp-named-parameter,readability-named-parameter)
  constexpr void push_back (T const&) noexcept { ++size_; }
  // NOLINTNEXTLINE(hicpp-named-parameter,readability-named-parameter)
  constexpr void push_back (T&&) noexcept { ++size_; }
  [[nodiscard]] constexpr std::size_t size () const noexcept { return size_; }
  [[nodiscard]] constexpr bool empty () const noexcept { return size_ == 0; }

private:
  std::size_t size_ = 0;
};

std::size_t pct_encoded_size (std::string_view const str,
                              pctencode_set const encodeset);

template <typename Function>
  requires std::is_invocable_r_v<std::string_view, Function, std::string_view,
                                 parts_field>
void parts_strings (parts& parts, Function const function) {
  if (parts.scheme.has_value () && parts.scheme->data () != nullptr) {
    parts.scheme = function (*parts.scheme, parts_field::scheme);
  }
  for (auto& segment : parts.path.segments) {
    if (segment.data () != nullptr) {
      segment = function (segment, parts_field::path);
    }
  }
  if (parts.authority.has_value ()) {
    auto& auth = *parts.authority;
    if (auth.userinfo.has_value () && auth.userinfo->data () != nullptr) {
      auth.userinfo = function (*auth.userinfo, parts_field::userinfo);
    }
    if (auth.host.data () != nullptr) {
      auth.host = function (auth.host, parts_field::host);
    }
    if (auth.port.has_value () && auth.port->data () != nullptr) {
      auth.port = function (*auth.port, parts_field::port);
    }
  }
  if (parts.query.has_value () && parts.query->data () != nullptr) {
    parts.query = function (*parts.query, parts_field::query);
  }
  if (parts.fragment.has_value () && parts.fragment->data () != nullptr) {
    parts.fragment = function (*parts.fragment, parts_field::fragment);
  }
}

template <std::output_iterator<char8_t> OutputIterator>
struct puny_encoded_result {
  OutputIterator out;
  bool any_non_ascii = false;
};

constexpr inline bool is_not_dot (char32_t const code_point) {
  return code_point != '.';
}

inline constexpr auto punycode_prefix = std::string_view{"xn--"};

template <std::ranges::input_range Range,
          std::output_iterator<char> OutputIterator>
  requires std::is_same_v<std::remove_cv_t<std::ranges::range_value_t<Range>>,
                          char32_t>
puny_encoded_result<OutputIterator> puny_encoded (Range&& range,
                                                  OutputIterator out) {
  using std::ranges::subrange;
  bool any_needed = false;
  auto const end = std::ranges::end (range);

  auto const find_dot = std::views::take_while (is_not_dot);
  auto view = subrange{std::ranges::begin (range), end} | find_dot;

  for (;;) {
    std::string segment;
    auto const& [in, _, any_non_ascii] =
      punycode::encode (view, true, std::back_inserter (segment));
    if (any_non_ascii) {
      out = std::ranges::copy (punycode_prefix, out).out;
      any_needed = true;
    }
    out = std::ranges::copy (segment, out).out;
    if (in == end) {
      break;
    }

    (*out++) = '.';
    view = subrange{std::next (in), end} | find_dot;
  }

  return {std::move (out), any_needed};
}

template <std::ranges::bidirectional_range Range,
          std::output_iterator<char32_t> OutputIterator>
  requires std::is_same_v<std::ranges::range_value_t<Range>, char>
std::error_code puny_decoded (Range&& range, OutputIterator out) {
  using namespace std::string_view_literals;
  using std::ranges::subrange;
  auto const end = std::ranges::end (range);

  auto const find_dot = std::views::take_while (is_not_dot);
  auto view = subrange{std::ranges::begin (range), end} | find_dot;

  for (;;) {
    if (starts_with (view, punycode_prefix)) {
      auto b = std::ranges::begin (view);
      std::ranges::advance (b, punycode_prefix.size ());

      auto const component = subrange{b, std::ranges::end (view)};
      std::string str2;
      std::ranges::copy (component, std::back_inserter (str2));

      auto const decode_res = punycode::decode (str2);
      if (auto const* erc = std::get_if<std::error_code> (&decode_res)) {
        return *erc;
      }
      auto const& [_, out_] =
        std::ranges::copy (std::get<std::u32string> (decode_res), out);
      out = std::move (out_);

      std::ranges::advance (b, str2.size () + 1);
      auto in = b;
      if (in == end) {
        break;
      }
      view = subrange{in, end} | find_dot;
    } else {
      auto const& [in, out_] = std::ranges::copy (view, out);
      out = out_;
      if (in == end) {
        break;
      }
      view = subrange{std::next (in), end} | find_dot;
    }

    (*out++) = '.';
  }

  return {};
}

template <std::ranges::input_range Range>
  requires std::is_same_v<
    std::remove_cv_t<typename std::ranges::range_value_t<Range>>, char32_t>
std::size_t puny_encoded_size (Range&& range) {
  details::ro_sink_container<char> sink;
  return puny_encoded (range, std::back_inserter (sink)).any_non_ascii
           ? sink.size ()
           : std::size_t{0};
}

}  // end namespace details

template <typename VectorContainer>
  requires std::contiguous_iterator<typename VectorContainer::iterator> &&
           std::is_same_v<typename VectorContainer::value_type, char>
parts encode (VectorContainer& store, parts const& p) {
  parts result = p;
  store.clear ();
  auto convert_to_utf32 = [] (auto r) {
    return r | std::views::transform ([] (char const c) {
             return static_cast<char8_t> (c);
           }) |
           icubaby::views::transcode<char8_t, char32_t>;
  };

  auto required_size = std::size_t{0};
  details::parts_strings (
    result, [&required_size, &convert_to_utf32] (std::string_view const str,
                                                 parts_field const field) {
      assert (str.data () != nullptr);
      required_size += field == parts_field::host
                         ? details::puny_encoded_size (convert_to_utf32 (str))
                         : details::pct_encoded_size (
                             str, pctencode_set_from_parts_field (field));
      return str;
    });
  store.reserve (required_size);

  details::parts_strings (
    result, [&store, &convert_to_utf32] (std::string_view const str,
                                         parts_field const field) {
      assert (str.data () != nullptr);
      auto const original_size = store.size ();
      if (field == parts_field::host) {
        auto const pes = details::puny_encoded_size (convert_to_utf32 (str));
        if (pes == 0) {
          return str;
        }
        assert (store.capacity () >= original_size + pes &&
                "store capacity is insufficient");
        details::puny_encoded (convert_to_utf32 (str),
                               std::back_inserter (store));
      } else {
        auto const es = pctencode_set_from_parts_field (field);
        if (!needs_pctencode (str, es)) {
          return str;
        }
        assert (store.capacity () >=
                  original_size + details::pct_encoded_size (str, es) &&
                "store capacity is insufficient");
        pctencode (std::begin (str), std::end (str), std::back_inserter (store),
                   es);
      }
      return std::string_view{store.data () + original_size,
                              store.size () - original_size};
    });
  assert (required_size == store.size () &&
          "Expected store required size does not match actual");
  return result;
}

}  // end namespace uri

#endif  // URI_PARTS_HPP
