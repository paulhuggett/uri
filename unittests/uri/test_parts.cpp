//===- unittests/uri/test_parts.cpp ---------------------------------------===//
//*                   _        *
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

#include <gmock/gmock.h>

#include "uri/icubaby.hpp"
#include "uri/parts.hpp"

#if URI_FUZZTEST
#include <fuzztest/fuzztest.h>
#endif

using namespace std::string_view_literals;

// NOLINTNEXTLINE
TEST (Puny, EncodedSizeNoNonAscii) {
  EXPECT_EQ (uri::details::puny_encoded_size (
               u8"a.b"sv | icubaby::views::transcode<char8_t, char32_t>),
             std::size_t{0});
}

// NOLINTNEXTLINE
TEST (Puny, EncodedThreeParts) {
  std::string output;
  uri::details::puny_encoded (
    u8"aaa.bbb.ccc"sv | icubaby::views::transcode<char8_t, char32_t>,
    std::back_inserter (output));
  EXPECT_EQ (output, "aaa.bbb.ccc");
}

// NOLINTNEXTLINE
TEST (Puny, EncodedMuchenDe) {
  std::string output;
  uri::details::puny_encoded (
    u8"M\xC3\xBCnchen.de"sv | icubaby::views::transcode<char8_t, char32_t>,
    std::back_inserter (output));
  EXPECT_EQ (output, "xn--Mnchen-3ya.de");
}

// NOLINTNEXTLINE
TEST (Puny, EncodedMuchenDotGrinningFace) {
  std::string output;
  // M<U+00FC LATIN SMALL LETTER U WITH DIAERESIS>nchen.<U+03C0 greek small
  // letter pi>
  uri::details::puny_encoded (u8"M\xC3\xBCnchen.\xCF\x80"sv |
                                icubaby::views::transcode<char8_t, char32_t>,
                              std::back_inserter (output));
  EXPECT_EQ (output, "xn--Mnchen-3ya.xn--1xa");
}

// NOLINTNEXTLINE
TEST (Puny, EncodedSizeMuchenDe) {
  EXPECT_EQ (
    uri::details::puny_encoded_size (
      u8"M\xC3\xBCnchen.de"sv | icubaby::views::transcode<char8_t, char32_t>),
    std::size_t{17});
}

// NOLINTNEXTLINE
TEST (Puny, EncodedMunchenGrinningFace) {
  uri::parts p;
  p.authority = (struct uri::parts::authority){
    std::nullopt, "M\xC3\xBCnchen.\xF0\x9F\x98\x80"sv, std::nullopt};
  std::vector<char> store;
  uri::parts encoded_parts = uri::encode (store, p);
  EXPECT_EQ (encoded_parts.authority->host, "xn--Mnchen-3ya.xn--e28h");
}

// NOLINTNEXTLINE
TEST (Puny, Decoded) {
  std::u32string output;
  auto const input = std::string_view{"aaa.bbb.ccc"};
  std::error_code const erc =
    uri::details::puny_decoded (input, std::back_inserter (output));
  EXPECT_FALSE (erc) << "Error was: " << erc.message ();
  EXPECT_THAT (
    output, testing::ElementsAre (char32_t{'a'}, char32_t{'a'}, char32_t{'a'},
                                  char32_t{'.'}, char32_t{'b'}, char32_t{'b'},
                                  char32_t{'b'}, char32_t{'.'}, char32_t{'c'},
                                  char32_t{'c'}, char32_t{'c'}));
}

// NOLINTNEXTLINE
TEST (Puny, DecodedMuchenDe) {
  std::u32string output;
  auto const input = std::string_view{"xn--Mnchen-3ya.de"};
  std::error_code const erc =
    uri::details::puny_decoded (input, std::back_inserter (output));
  EXPECT_FALSE (erc) << "Error was: " << erc.message ();

  static constexpr auto latin_small_letter_u_with_diaresis = char32_t{0x00FC};
  EXPECT_THAT (
    output, testing::ElementsAre (
              char32_t{'M'}, latin_small_letter_u_with_diaresis, char32_t{'n'},
              char32_t{'c'}, char32_t{'h'}, char32_t{'e'}, char32_t{'n'},
              char32_t{'.'}, char32_t{'d'}, char32_t{'e'}));
}

#if URI_FUZZTEST
using opt_authority = std::optional<struct uri::parts::authority>;

struct parts_without_authority {
  std::optional<std::string> scheme;
  struct uri::parts::path path;
  std::optional<std::string> query;
  std::optional<std::string> fragment;

  [[nodiscard]] uri::parts as_parts (opt_authority&& auth) const {
    // assert (!auth || auth->host.data () != nullptr);
    return uri::parts{scheme, std::move (auth), path, query, fragment};
  }
};
static void EncodeAndComposeValidAlwaysAgree (
  parts_without_authority const& base, opt_authority&& auth) {
  std::vector<char> store;
  uri::parts const p = uri::encode (store, base.as_parts (std::move (auth)));
  if (p.valid ()) {
    std::string const str = uri::compose (p);
    EXPECT_TRUE (uri::split (str).has_value ());
  }
}
// NOLINTNEXTLINE
FUZZ_TEST (Puny, EncodeAndComposeValidAlwaysAgree);
#endif  // URI_FUZZTEST
