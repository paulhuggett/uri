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
TEST (Parts, PunyEncodedSizeNoNonAscii) {
  EXPECT_EQ (uri::details::puny_encoded_size (
               u8"a.b"sv | icubaby::views::transcode<char8_t, char32_t>),
             std::size_t{0});
}

// NOLINTNEXTLINE
TEST (Parts, PunyDecodedSizeNoNonAscii) {
  std::variant<std::error_code, std::size_t> const puny_decoded_result =
    uri::details::puny_decoded_size ("a.b"sv);
  ASSERT_TRUE (std::holds_alternative<std::size_t> (puny_decoded_result));
  EXPECT_EQ (std::get<std::size_t> (puny_decoded_result), std::size_t{0});
}

// NOLINTNEXTLINE
TEST (Parts, PunyEncodedThreeParts) {
  std::string output;
  uri::details::puny_encoded (
    u8"aaa.bbb.ccc"sv | icubaby::views::transcode<char8_t, char32_t>,
    std::back_inserter (output));
  EXPECT_EQ (output, "aaa.bbb.ccc");
}

// NOLINTNEXTLINE
TEST (Parts, PunyEncodedMuchenDe) {
  std::string output;
  uri::details::puny_encoded (
    u8"M\xC3\xBCnchen.de"sv | icubaby::views::transcode<char8_t, char32_t>,
    std::back_inserter (output));
  EXPECT_EQ (output, "xn--Mnchen-3ya.de");
}

// NOLINTNEXTLINE
TEST (Parts, PunyEncodedMuchenDotGrinningFace) {
  std::string output;
  // M<U+00FC LATIN SMALL LETTER U WITH DIAERESIS>nchen.<U+03C0 greek small
  // letter pi>
  uri::details::puny_encoded (u8"M\xC3\xBCnchen.\xCF\x80"sv |
                                icubaby::views::transcode<char8_t, char32_t>,
                              std::back_inserter (output));
  EXPECT_EQ (output, "xn--Mnchen-3ya.xn--1xa");
}

// NOLINTNEXTLINE
TEST (Parts, PunyEncodedSizeMuchenDe) {
  EXPECT_EQ (
    uri::details::puny_encoded_size (
      u8"M\xC3\xBCnchen.de"sv | icubaby::views::transcode<char8_t, char32_t>),
    std::size_t{17});
}

// NOLINTNEXTLINE
TEST (Parts, PunyEncodedMunchenGrinningFace) {
  uri::parts p;
  p.authority = (struct uri::parts::authority){
    std::nullopt, "M\xC3\xBCnchen.\xF0\x9F\x98\x80"sv, std::nullopt};
  std::vector<char> store;
  uri::parts encoded_parts = uri::encode (store, p);
  EXPECT_EQ (encoded_parts.authority->host, "xn--Mnchen-3ya.xn--e28h");
}

// NOLINTNEXTLINE
TEST (Parts, PunyDecoded) {
  std::u32string output;
  auto out = std::back_inserter (output);
  auto const input = std::string_view{"aaa.bbb.ccc"};

  auto const puny_decoded_result = uri::details::puny_decoded (input, out);

  using result_type = uri::details::puny_decoded_result<decltype (out)>;
  ASSERT_TRUE (std::holds_alternative<result_type> (puny_decoded_result));
  auto const& result = std::get<result_type> (puny_decoded_result);
  EXPECT_FALSE (result.any_encoded);
  EXPECT_THAT (
    output, testing::ElementsAre (char32_t{'a'}, char32_t{'a'}, char32_t{'a'},
                                  char32_t{'.'}, char32_t{'b'}, char32_t{'b'},
                                  char32_t{'b'}, char32_t{'.'}, char32_t{'c'},
                                  char32_t{'c'}, char32_t{'c'}));
}

// NOLINTNEXTLINE
TEST (Parts, PunyDecodedMuchenDe) {
  std::vector<char8_t> output;
  auto out = std::back_inserter (output);
  auto const input = std::string_view{"xn--Mnchen-3ya.de"};
  auto const puny_decoded_result = uri::details::puny_decoded (input, out);

  using result_type = uri::details::puny_decoded_result<decltype (out)>;
  ASSERT_TRUE (std::holds_alternative<result_type> (puny_decoded_result));
  auto const& result = std::get<result_type> (puny_decoded_result);
  EXPECT_TRUE (result.any_encoded);

  // latin-small-letter-u-with-diaresis is U+00FC (C3 BC)
  EXPECT_THAT (output,
               testing::ElementsAre (
                 'M', static_cast<char8_t> (0xC3), static_cast<char8_t> (0xBC),
                 'n', char8_t{'c'}, char8_t{'h'}, char8_t{'e'}, char8_t{'n'},
                 char8_t{'.'}, char8_t{'d'}, char8_t{'e'}));
}

// NOLINTNEXTLINE
TEST (Parts, AllSetButNothingToEncode) {
  uri::parts input;
  input.scheme = "https"sv;
  input.authority =
    (struct uri::parts::authority){"user"sv, "host"sv, "1234"sv};
  input.path.absolute = true;
  input.path.segments = std::vector<std::string_view>{"a"sv, "b"sv};
  input.query = "query"sv;
  input.fragment = "fragment"sv;
  ASSERT_TRUE (input.valid ());

  std::vector<char> store;
  uri::parts const output = uri::encode (store, input);

  EXPECT_TRUE (output.valid ());
  EXPECT_EQ (store.size (), 0U)
    << "Nothing to encode so store size should be 0";
  EXPECT_EQ (output.scheme, input.scheme);
  ASSERT_TRUE (output.authority.has_value ());
  EXPECT_EQ (output.authority->userinfo, input.authority->userinfo);
  EXPECT_EQ (output.authority->host, input.authority->host);
  EXPECT_EQ (output.authority->port, input.authority->port);
  EXPECT_EQ (output.path.absolute, input.path.absolute);
  EXPECT_THAT (output.path.segments,
               testing::ContainerEq (input.path.segments));
  ASSERT_TRUE (output.query.has_value ());
  EXPECT_EQ (*output.query, *input.query);
  ASSERT_TRUE (output.fragment.has_value ());
  EXPECT_EQ (*output.fragment, *input.fragment);
}

// NOLINTNEXTLINE
TEST (Parts, EncodeDecode) {
  uri::parts original;
  original.scheme = "https"sv;
  original.authority =
    (struct uri::parts::authority){"user"sv, "M\xC3\xBCnchen.de"sv, "1234"sv};
  original.path.absolute = true;
  original.path.segments = std::vector<std::string_view>{"~\xC2\xA1"sv};
  original.query = "a%b"sv;
  original.fragment = "c%d"sv;

  std::vector<char> encode_store;
  uri::parts const encoded = uri::encode (encode_store, original);
  EXPECT_TRUE (encoded.valid ());

  std::vector<char> decode_store;
  std::variant<std::error_code, uri::parts> const decode_result =
    uri::decode (decode_store, encoded);
  ASSERT_TRUE (std::holds_alternative<uri::parts> (decode_result));

  auto const& decoded = std::get<uri::parts> (decode_result);
  EXPECT_EQ (decoded.scheme, original.scheme);
  ASSERT_TRUE (decoded.authority.has_value ());
  EXPECT_EQ (decoded.authority->userinfo, original.authority->userinfo);
  EXPECT_EQ (decoded.authority->host, original.authority->host);
  EXPECT_EQ (decoded.authority->port, original.authority->port);
  EXPECT_EQ (decoded.path.absolute, original.path.absolute);
  EXPECT_THAT (decoded.path.segments,
               testing::ContainerEq (original.path.segments));
  ASSERT_TRUE (decoded.query.has_value ());
  EXPECT_EQ (*decoded.query, *original.query);
  ASSERT_TRUE (decoded.fragment.has_value ());
  EXPECT_EQ (*decoded.fragment, *original.fragment);
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
  if (uri::parts const p =
        uri::encode (store, base.as_parts (std::move (auth)));
      p.valid ()) {
    std::string const str = uri::compose (p);
    EXPECT_TRUE (uri::split (str).has_value ());
  }
}
// NOLINTNEXTLINE
FUZZ_TEST (Parts, EncodeAndComposeValidAlwaysAgree);

static void EncodeDecodeRoundTrip (parts_without_authority const& base,
                                   opt_authority&& auth) {
  if (uri::parts const original = base.as_parts (std::move (auth));
      original.valid ()) {
    std::vector<char> encode_store;
    uri::parts const encoded = uri::encode (encode_store, original);
    EXPECT_TRUE (encoded.valid ());

    std::vector<char> decode_store;
    std::variant<std::error_code, uri::parts> const decode_result =
      uri::decode (decode_store, encoded);
    ASSERT_TRUE (std::holds_alternative<uri::parts> (decode_result));

    auto const& decoded = std::get<uri::parts> (decode_result);
    EXPECT_EQ (decoded.scheme, original.scheme);
    ASSERT_EQ (decoded.authority, original.authority);
    if (decoded.authority && original.authority) {
      EXPECT_EQ (decoded.authority->userinfo, original.authority->userinfo);
      EXPECT_EQ (decoded.authority->host, original.authority->host);
      EXPECT_EQ (decoded.authority->port, original.authority->port);
    }
    EXPECT_EQ (decoded.path.absolute, original.path.absolute);
    EXPECT_THAT (decoded.path.segments,
                 testing::ContainerEq (original.path.segments));
    EXPECT_EQ (decoded.query, original.query);
    EXPECT_EQ (decoded.fragment, original.fragment);
  }
}
// NOLINTNEXTLINE
FUZZ_TEST (Parts, EncodeDecodeRoundTrip);

#endif  // URI_FUZZTEST
