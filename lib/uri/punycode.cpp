//===- lib/uri/punycode.cpp -----------------------------------------------===//
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
#include "uri/punycode.hpp"

#include <cstdint>

namespace {

/// \returns The numeric value of a basic code point (for use in representing
///   integers) in the range 0 to base-1, or base if cp does not represent a
///   value.
constexpr unsigned decode_digit (std::uint_least8_t cp) noexcept {
  constexpr auto alphabet_size = 26U;
  if (cp >= '0' && cp <= '9') {
    // Digits 0..9 represent values 26..35
    return cp - ('0' - alphabet_size);
  }
  if (cp >= 'a') {
    cp -= 'a' - 'A';  // Convert to upper case.
  }
  return (cp >= 'A' && cp <= 'Z') ? cp - 'A' : uri::punycode::details::base;
}

constexpr std::size_t clampk (std::size_t const k,
                              std::size_t const bias) noexcept {
  using uri::punycode::details::tmax;
  using uri::punycode::details::tmin;
  if (k <= bias) {
    return tmin;
  }
  if (k >= bias + tmax) {
    return tmax;
  }
  return k - bias;
}

// Decode a generalized variable-length integer.
template <typename Iterator>
std::variant<std::error_code, std::tuple<std::size_t, Iterator>> decode_vli (
  Iterator first, Iterator last, std::size_t vli, std::size_t bias) {
  static constexpr auto max = std::numeric_limits<std::size_t>::max ();
  using uri::punycode::decode_error_code;
  using uri::punycode::details::base;

  auto w = std::size_t{1};
  for (auto k = base;; k += base) {
    if (first == last) {
      return make_error_code (decode_error_code::bad_input);
    }
    auto const digit = decode_digit (static_cast<std::uint_least8_t> (*first));
    assert (digit <= base);
    ++first;

    if (digit >= base) {
      return make_error_code (decode_error_code::bad_input);
    }
    if (digit > (max - vli) / w) {
      return make_error_code (decode_error_code::overflow);
    }
    vli += digit * w;
    std::size_t const t = clampk (k, bias);
    if (digit < t) {
      break;
    }
    if (w > max / (base - t)) {
      return make_error_code (decode_error_code::overflow);
    }
    w *= (base - t);
  }
  return std::make_tuple (vli, first);
}

}  // end anonymous namespace

namespace uri::punycode {

char const* error_category::name () const noexcept {
  return "punycode decode";
}
std::string error_category::message (int error) const {
  auto const* m = "unknown error";
  switch (static_cast<decode_error_code> (error)) {
  case decode_error_code::bad_input: m = "bad input"; break;
  case decode_error_code::overflow: m = "overflow"; break;
  case decode_error_code::none: m = "unknown error"; break;
  }
  return m;
}
std::error_code make_error_code (decode_error_code const e) {
  static error_category category;
  return {static_cast<int> (e), category};
}

decode_result decode (std::string_view const& input) {
  std::u32string output;
  static constexpr auto maxint =
    std::numeric_limits<std::uint_least32_t>::max ();
  using details::adapt;
  using details::delimiter;
  using details::initial_bias;
  using details::initial_n;

  // Find the end of the literal portion (if there is one) by scanning for the
  // last delimiter.
  auto rb = std::find (input.rbegin (), input.rend (), delimiter);
  // NOLINTNEXTLINE(llvm-qualified-auto,readability-qualified-auto)
  auto b = rb == input.rend () ? input.begin () : rb.base () - 1;
  // Copy the plain ASCII part of the string to the output (if any).
  // NOLINTNEXTLINE(llvm-qualified-auto,readability-qualified-auto)
  for (auto pos = input.begin (); pos != b; ++pos) {
    auto const code_point = static_cast<char32_t> (*pos);
    if (!details::is_basic_code_point (code_point)) {
      return make_error_code (decode_error_code::bad_input);
    }
    output.push_back (code_point);
  }

  // The main decoding loop.
  auto n = initial_n;
  auto i = std::size_t{0};
  auto bias = initial_bias;
  // Start just after the last delimiter if any basic code points were
  // copied; start at the beginning otherwise. *in is the next character to be
  // consumed.
  // NOLINTNEXTLINE(llvm-qualified-auto,readability-qualified-auto)
  auto in = b != input.begin () ? b + 1 : input.begin ();
  while (in != input.end ()) {
    // Decode a generalized variable-length integer into delta, which gets added
    // to i. The overflow checking is easier if we increase i as we go, then
    // subtract off its starting value at the end to obtain delta.
    auto const decode_res = decode_vli (in, input.end (), i, bias);
    static_assert (
      std::is_same_v<std::error_code,
                     std::remove_const_t<
                       std::variant_alternative_t<0, decltype (decode_res)>>>);
    if (auto const* err = std::get_if<std::error_code> (&decode_res)) {
      return *err;
    }
    auto const old_vli = i;
    auto output_length = output.length ();
    std::tie (i, in) = std::get<1> (decode_res);
    bias = adapt (i - old_vli, output_length + 1, old_vli == 0);

    // i was supposed to wrap around from out+1 to 0, incrementing n each time,
    // so we'll fix that now.
    if (i / (output_length + 1) > maxint - n) {
      return make_error_code (decode_error_code::overflow);
    }
    n += i / (output_length + 1);
    i %= (output_length + 1);

    // Insert n into the output at position i.
    output.insert (i, std::size_t{1}, static_cast<char32_t> (n));
    ++i;
  }
  return output;
}

}  // end namespace uri::punycode
