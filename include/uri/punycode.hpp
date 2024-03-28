//===- include/uri/punycode.hpp ---------------------------*- mode: C++ -*-===//
//*                                        _       *
//*  _ __  _   _ _ __  _   _  ___ ___   __| | ___  *
//* | '_ \| | | | '_ \| | | |/ __/ _ \ / _` |/ _ \ *
//* | |_) | |_| | | | | |_| | (_| (_) | (_| |  __/ *
//* | .__/ \__,_|_| |_|\__, |\___\___/ \__,_|\___| *
//* |_|                |___/                       *
//===----------------------------------------------------------------------===//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//
#ifndef URI_PUNYCODE_HPP
#define URI_PUNYCODE_HPP

#include <algorithm>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <string_view>
#include <system_error>
#include <tuple>
#include <variant>

namespace uri::punycode {

enum class decode_error_code : int {
  none,
  bad_input,
  overflow,
};

class error_category final : public std::error_category {
public:
  char const* name () const noexcept override;
  std::string message (int error) const override;
};
std::error_code make_error_code (decode_error_code e);

namespace details {

constexpr auto damp = 700U;
constexpr auto base = 36U;
constexpr auto tmin = 1U;
constexpr auto tmax = 26U;
constexpr auto skew = 38U;
constexpr auto initial_bias = std::size_t{72};
constexpr auto initial_n = std::size_t{0x80};
constexpr auto delimiter = char{0x2D}; // U+002D HYPHEN-MINUS

/// \param c  The code-point to be checked.
/// \returns true if the \p c represents a "basic" code-point. That is,
///   a code-point less than U+0080.
constexpr bool is_basic_code_point (char32_t const c) noexcept {
  return c < 0x80;
}

/// \param d  A value in the range [0,base) to be encoded as an ASCII character.
/// \returns The basic code point whose value (when used for representing
///   integers) is d, which needs to be in the range 0 to base-1. The lowercase
///   form is used.
constexpr char encode_digit (unsigned const d) noexcept {
  //  0..25 maps to ASCII a..z; 26..35 maps to ASCII 0..9
  assert (d < 36U);
  static_assert (base == 36U);
  if (d < 26U) {
    return static_cast<char> (d + static_cast<unsigned> ('a'));
  }
  return static_cast<char> (d - 26U + static_cast<unsigned> ('0'));
}

constexpr std::size_t clamp (std::size_t const k,
                             std::size_t const bias) noexcept {
  if (k <= bias) {
    return tmin;
  }
  if (k >= bias + tmax) {
    return tmax;
  }
  return k - bias;
}

template <typename OutputIterator>
OutputIterator encode_vli (std::size_t q, std::size_t const bias,
                           OutputIterator out) {
  for (auto k = base;; k += base) {
    auto const t = clamp (k, bias);
    if (q < t) {
      break;
    }
    assert (base >= t);
    *(out++) = encode_digit (static_cast<unsigned> (t + (q - t) % (base - t)));
    q = (q - t) / (base - t);
  }
  *(out++) = encode_digit (static_cast<unsigned> (q));
  return out;
}

constexpr std::size_t adapt (std::size_t delta, std::size_t const numpoints,
                             bool const firsttime) {
  delta = firsttime ? delta / damp : delta >> 1U;
  delta += delta / numpoints;
  auto k = 0U;
  while (delta > ((base - tmin) * tmax) / 2U) {
    delta = delta / (base - tmin);
    k += base;
  }
  return k + (base - tmin + 1) * delta / (delta + skew);
}

template <typename Container>
void sort_and_remove_duplicates (Container& container) {
  auto const first = container.begin ();
  auto const last = container.end ();
  std::sort (first, last);
  container.erase (std::unique (first, last), last);
}

}  // namespace details

template <typename OutputIterator>
OutputIterator encode (std::u32string_view const& input,
                       OutputIterator output) {
  std::u32string nonbasic;
  auto num_basics = std::size_t{0};
  // Handle the basic code points. Copy them to the output in order followed by
  // a delimiter if any were copied.
  for (auto cp : input) {
    if (details::is_basic_code_point (cp)) {
      *(output++) = static_cast<char> (cp);
      ++num_basics;
    } else {
      nonbasic += cp;
    }
  }
  details::sort_and_remove_duplicates (nonbasic);
  auto i = num_basics;
  if (num_basics > 0) {
    *(output++) = details::delimiter;
  }
  auto n = details::initial_n;
  auto delta = std::size_t{0};
  auto bias = details::initial_bias;
  for (char32_t const m : nonbasic) {
    assert (m >= n);
    delta += (m - n) * (i + 1);
    n = m;
    // for each code point c in the input (in order)
    for (char32_t const c : input) {
      if (c < n) {
        ++delta;  // increment delta (fail on overflow)
      } else if (c == n) {
        // Represent delta as a generalized variable-length integer.
        output = details::encode_vli (delta, bias, output);
        bias = details::adapt (delta, i + 1, i == num_basics);
        delta = 0U;
        ++i;
      }
    }
    ++delta;
    ++n;
  }
  return output;
}

using decode_result = std::variant<std::error_code, std::u32string>;
decode_result decode (std::string_view const& input);

}  // end namespace uri::punycode

#endif  // URI_PUNYCODE_HPP
