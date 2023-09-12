//===- include/uri/pct_decode.hpp -------------------------*- mode: C++ -*-===//
//*             _         _                    _       *
//*  _ __   ___| |_    __| | ___  ___ ___   __| | ___  *
//* | '_ \ / __| __|  / _` |/ _ \/ __/ _ \ / _` |/ _ \ *
//* | |_) | (__| |_  | (_| |  __/ (_| (_) | (_| |  __/ *
//* | .__/ \___|\__|  \__,_|\___|\___\___/ \__,_|\___| *
//* |_|                                                *
//===----------------------------------------------------------------------===//
//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE.TXT
// for license information.
// SPDX-License-Identifier: MIT
//
//===----------------------------------------------------------------------===//
#ifndef URI_PCT_DECODE_HPP
#define URI_PCT_DECODE_HPP

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstddef>
#include <iterator>
#include <string_view>

#if __has_include(<version>)
#include <version>
#endif

#if defined(__cpp_concepts) && defined(__cpp_lib_concepts)
#include <concepts>
#endif
#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L
#include <ranges>
#endif

namespace uri {

/// pct_decode_iterator is a forward-iterator which will produce characters from
/// a string-view instance. Each time that it encounters a percent character "%"
/// followed by two hexadecimal digits, the hexadecimal value is decoded. For
/// example, "%20" is the percent-encoding for character 32 which in US-ASCII
/// corresponds to the space character (SP). The uppercase hexadecimal digits
/// 'A' through 'F' are equivalent to the lowercase digits 'a' through 'f',
/// respectively.
///
/// If the two characters following the percent character are _not_ valid
/// hexadecimal digits, the text is left unchanged.
#ifdef __cpp_concepts
template <std::integral CharT>
#else
template <typename CharT,
          typename = std::enable_if_t<std::is_integral_v<CharT>>>
#endif
class pct_decode_iterator {
  using string_view = std::basic_string_view<CharT>;

public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = CharT;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type const*;
  using reference = value_type const&;

  constexpr pct_decode_iterator () noexcept = default;
  explicit constexpr pct_decode_iterator (string_view str) noexcept
      : str_{str} {}

#if __cplusplus < 202002L
  constexpr bool operator== (pct_decode_iterator const& other) const noexcept {
    return str_ == other.str_;
  }
  constexpr bool operator!= (pct_decode_iterator const& other) const noexcept {
    return str_ != other.str_;
  }
#else
  constexpr bool operator== (pct_decode_iterator const& other) const noexcept =
    default;
#endif

  reference operator* () const {
    reference c = str_[0];
    if (c != '%' || str_.length () < 3) {
      return c;
    }
    auto const nhi = hex2dec (str_[1]);
    auto const nlo = hex2dec (str_[2]);
    // If either character isn't valid hex, then return the original.
    if (either_bad (nhi, nlo)) {
      return c;
    }
    hex_ = static_cast<value_type> ((nhi << 4) | nlo);
    return hex_;
  }
  pointer operator->() const { return &(**this); }

  pct_decode_iterator& operator++ () {
    using size_type = std::string_view::size_type;
    // Remove 1 character unless we've got a '%' followed by two legal hex
    // characters in which case we remove 3.
    str_.remove_prefix (str_.length () >= 3 && str_[0] == '%' &&
                            !either_bad (hex2dec (str_[1]), hex2dec (str_[2]))
                          ? size_type{3}
                          : size_type{1});
    return *this;
  }
  pct_decode_iterator operator++ (int) {
    auto const prev = *this;
    ++(*this);
    return prev;
  }

  constexpr string_view str () const noexcept { return str_; }

private:
  static constexpr std::byte bad = std::byte{0b1'0000};
  /// Convert the argument character from a hexadecimal character code
  /// (A-F/a-f/0-9) to an integer in the range 0-15. If the input character is
  /// not a valid hex character code, returns pct_decode::bad.
  static constexpr std::byte hex2dec (value_type const digit) noexcept {
    if (digit >= 'a' && digit <= 'f') {
      return static_cast<std::byte> (static_cast<unsigned> (digit) -
                                     ('a' - 10));
    }
    if (digit >= 'A' && digit <= 'F') {
      return static_cast<std::byte> (static_cast<unsigned> (digit) -
                                     ('A' - 10));
    }
    if (digit >= '0' && digit <= '9') {
      return static_cast<std::byte> (static_cast<unsigned> (digit) - '0');
    }
    return bad;
  }
  static bool either_bad (std::byte n1, std::byte n2) noexcept {
    return ((n1 | n2) & bad) != std::byte{0};
  }

  string_view str_;
  mutable value_type hex_ = 0;
};

template <typename CharT>
pct_decode_iterator (std::basic_string_view<CharT>)
  -> pct_decode_iterator<CharT>;

template <typename CharT>
constexpr auto pct_decode_begin (std::basic_string_view<CharT> str) noexcept {
  return pct_decode_iterator<CharT>{str};
}
template <typename CharT>
constexpr auto pct_decode_end (std::basic_string_view<CharT> str) noexcept {
  return pct_decode_iterator{str.substr (str.length ())};
}

template <typename CharT>
class pct_decoder {
public:
  explicit constexpr pct_decoder (std::basic_string_view<CharT> str) noexcept
      : begin_{pct_decode_begin (str)}, end_{pct_decode_end (str)} {}
  constexpr auto begin () const { return begin_; }
  constexpr auto end () const { return end_; }

private:
  pct_decode_iterator<CharT> begin_;
  pct_decode_iterator<CharT> end_;
};

template <typename CharT>
pct_decoder (std::basic_string_view<CharT>) -> pct_decoder<CharT>;
template <typename CharT>
pct_decoder (std::basic_string<CharT>) -> pct_decoder<CharT>;
template <typename CharT>
pct_decoder (CharT const*) -> pct_decoder<CharT>;

#ifdef __cpp_concepts
template <std::integral CharT>
#else
template <typename CharT,
          typename = std::enable_if_t<std::is_integral_v<CharT>>>
#endif
class pct_decode_lower_iterator {
public:
  using iterator_category = std::forward_iterator_tag;
  using value_type = CharT;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type const*;
  using reference = value_type const&;

  constexpr pct_decode_lower_iterator () noexcept = default;
  explicit constexpr pct_decode_lower_iterator (
    std::basic_string_view<CharT> str) noexcept
      : it_{str} {}

  constexpr bool operator== (
    pct_decode_lower_iterator const& other) const noexcept {
    return it_ == other.it_;
  }
#if __cplusplus < 202002L
  constexpr bool operator!= (
    pct_decode_lower_iterator const& other) const noexcept {
    return it_ != other.it_;
  }
#endif
  reference operator* () const {
    c_ = static_cast<CharT> (std::tolower (static_cast<int> (*it_)));
    return c_;
  }
  pointer operator->() const { return &(**this); }
  pct_decode_lower_iterator& operator++ () {
    ++it_;
    return *this;
  }
  pct_decode_lower_iterator operator++ (int) {
    auto prev = *this;
    ++(*this);
    return prev;
  }

private:
  pct_decode_iterator<CharT> it_;
  mutable value_type c_ = '\0';
};

template <typename CharT>
pct_decode_lower_iterator (std::basic_string_view<CharT>)
  -> pct_decode_lower_iterator<CharT>;

template <typename CharT>
class pct_decoder_lower {
public:
  explicit constexpr pct_decoder_lower (
    std::basic_string_view<CharT> str) noexcept
      : begin_{pct_decode_lower_iterator{str}},
        end_{pct_decode_lower_iterator{str.substr (str.length ())}} {}
  constexpr auto begin () const { return begin_; }
  constexpr auto end () const { return end_; }

private:
  pct_decode_lower_iterator<CharT> begin_;
  pct_decode_lower_iterator<CharT> end_;
};

template <typename CharT>
pct_decoder_lower (std::basic_string_view<CharT>) -> pct_decoder_lower<CharT>;

#if defined(__cpp_lib_ranges) && __cpp_lib_ranges >= 201811L
template <typename CharT>
constexpr bool operator== (pct_decode_iterator<CharT> const& d,
                           std::default_sentinel_t) noexcept {
  return d.str ().empty ();
}
template <typename CharT>
constexpr bool operator== (std::default_sentinel_t,
                           pct_decode_iterator<CharT> const& d) noexcept {
  return d.str ().empty ();
}

template <typename CharT>
constexpr bool operator!= (pct_decode_iterator<CharT> const& d,
                           std::default_sentinel_t) noexcept {
  return !d.str ().empty ();
}
template <typename CharT>
constexpr bool operator!= (std::default_sentinel_t,
                           pct_decode_iterator<CharT> const& d) noexcept {
  return !d.str ().empty ();
}

template <std::ranges::view View, std::basic_string_view StrView>
  class pct_decode_view
    : public std::ranges::view_interface < pct_decode_view<View, StrView> {
public:
  constexpr pct_decode_view () noexcept = default;
  constexpr pct_decode_view (View base)
      : base_{std::move (base)},
        begin_{pct_decode_begin (base)},
        end_{pct_decode_end (str)} {}

  constexpr auto begin () const noexcept { return begin_; }
  constexpr auto end () const noexcept { return end_; }

private:
  View base_;
  pct_decode_iterator<CharT> begin_;
  pct_decode_iterator<CharT> end_;
};

struct pct_decode_fn {
  template <std::ranges::viewable_range R, std::basic_string_view StrView>
  constexpr auto operator() (R&& r, StrView s) const
    -> pct_decode_view<std::views::all_t<R>, StrView> {
    retrun pct_decode_view<std::views::all_t<R>, StrView> (std::views::all (std::forward<R>(r), std::move (s));
  }
};

inline constexpr auto pct_decode = pct_decode_fn{};

#endif  // __cpp_lib_ranges

}  // end namespace uri

#endif  // URI_PCT_DECODE_HPP
