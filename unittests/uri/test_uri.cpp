//===- unittests/uri/test_uri.cpp -----------------------------------------===//
//*             _  *
//*  _   _ _ __(_) *
//* | | | | '__| | *
//* | |_| | |  | | *
//*  \__,_|_|  |_| *
//*                *
//===----------------------------------------------------------------------===//
//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE.TXT
// for license information.
// SPDX-License-Identifier: MIT
//
//===----------------------------------------------------------------------===//

#include <gmock/gmock.h>

#include "uri/uri.hpp"

using testing::ElementsAre;
using namespace std::string_view_literals;

// The tests in this group were generated using the 'abnfgen' tool with the
// official URI ABNF. The tool was asked to produce 100 tests with the "attempt
// full coverage" (-c) mode enabled.

// NOLINTNEXTLINE
TEST (Uri, Empty) {
  std::optional<uri::parts> const x = uri::split ("");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->scheme);
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, EmptyPathComponents) {
  std::optional<uri::parts> const x = uri::split ("/foo///bar");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->scheme);
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("foo", "", "", "bar"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}

// NOLINTNEXTLINE
TEST (Uri, 0001) {
  auto const x = uri::split ("C://[::A:eE5c]:2194/&///@//:_/%aB//.////#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "C");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[::A:eE5c]");
  EXPECT_EQ (x->authority.port, "2194");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("&", "", "", "@", "", ":_", "%aB",
                                              "", ".", "", "", "", ""));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
  EXPECT_EQ (static_cast<std::string> (x->path), "/&///@//:_/%aB//.////");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path).generic_string (),
             "/&/@/:_/%aB/./");
}
// NOLINTNEXTLINE
TEST (Uri, 0002) {
  auto const x = uri::split ("P-.:/?/?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P-.");
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
  EXPECT_EQ (x->query, "/?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0003) {
  auto const x = uri::split ("i+V:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "i+V");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0004) {
  auto const x = uri::split ("L:%Cf#%dD/?H");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("%Cf"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "%dD/?H");
  EXPECT_EQ (static_cast<std::string> (x->path), "%Cf");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path).generic_string (),
             "%Cf");
}
// NOLINTNEXTLINE
TEST (Uri, 0005) {
  auto const x = uri::split ("E07:/8=-~%bF//%36////'/%16N%78//)/%53/;?*!");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "E07");
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments,
               ElementsAre ("8=-~%bF", "", "%36", "", "", "", "'", "%16N%78",
                            "", ")", "%53", ";"));
  EXPECT_EQ (x->query, "*!");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0006) {
  auto const x = uri::split ("v:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0007) {
  auto const x = uri::split ("YXa:/#B");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "YXa");
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "B");
}
// NOLINTNEXTLINE
TEST (Uri, 0008) {
  auto const x = uri::split ("n:/,+?$#(+!)D");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "n");
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (",+"));
  EXPECT_EQ (x->query, "$");
  EXPECT_EQ (x->fragment, "(+!)D");
}
// NOLINTNEXTLINE
TEST (Uri, 0009) {
  auto const x = uri::split ("m:/?cJ");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "m");
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
  EXPECT_EQ (x->query, "cJ");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0010) {
  auto const x = uri::split ("zR:d/M/kx/s/GTl///SgA/?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "zR");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments,
               ElementsAre ("d", "M", "kx", "s", "GTl", "", "", "SgA", ""));
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0011) {
  auto const x = uri::split ("t:W?p#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "t");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("W"));
  EXPECT_EQ (x->query, "p");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0012) {
  auto const x = uri::split ("QrIq:/#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "QrIq");
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0013) {
  auto const x = uri::split ("OuU:/?bZK");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "OuU");
  EXPECT_FALSE (x->authority);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
  EXPECT_EQ (x->query, "bZK");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0014) {
  auto const x = uri::split ("Fjfe:?h");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Fjfe");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "h");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0015) {
  auto const x = uri::split ("y:w/o/b/?lKTF");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "y");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("w", "o", "b", ""));
  EXPECT_EQ (x->query, "lKTF");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0016) {
  auto const x = uri::split ("P://=:_%bb%Cf%2F-8;~@230.109.31.250#.");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P");
  EXPECT_EQ (x->authority.userinfo, "=:_%bb%Cf%2F-8;~");
  EXPECT_EQ (x->authority.host, "230.109.31.250");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, ".");
}
// NOLINTNEXTLINE
TEST (Uri, 0017) {
  auto const x = uri::split ("N://@=i%bD%Cb&*%Ea)%CE//:%cA//#?//");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "N");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "=i%bD%Cb&*%Ea)%CE");
  EXPECT_FALSE (x->authority.port);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("", ":%cA", "", ""));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "?//");
  EXPECT_EQ (static_cast<std::string> (x->path), "//:%cA//");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/:%cA/");
}
// NOLINTNEXTLINE
TEST (Uri, 0018) {
  auto const x = uri::split ("X:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "X");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0019) {
  auto const x = uri::split ("U:??");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "U");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0020) {
  auto const x = uri::split ("G:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "G");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0021) {
  auto const x = uri::split ("l6+:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "l6+");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0022) {
  auto const x = uri::split ("T.-://:@[VD.~]:?/@#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "T.-");
  EXPECT_EQ (x->authority.userinfo, ":");
  EXPECT_EQ (x->authority.host, "[VD.~]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "/@");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0023) {
  auto const x = uri::split ("rC://3.76.206.5:8966?/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "rC");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "3.76.206.5");
  EXPECT_EQ (x->authority.port, "8966");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0024) {
  auto const x = uri::split ("oNP:///::");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "oNP");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "");
  EXPECT_FALSE (x->authority.port);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("::"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0025) {
  auto const x = uri::split ("g0:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "g0");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0026) {
  auto const x = uri::split ("Do1-:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Do1-");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0027) {
  auto const x = uri::split ("K:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "K");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0028) {
  auto const x = uri::split ("tc://@[::F]:/::@~?@/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "tc");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "[::F]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("::@~"));
  EXPECT_EQ (x->query, "@/");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0029) {
  auto const x = uri::split ("N:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "N");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0030) {
  auto const x = uri::split ("o:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "o");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0031) {
  auto const x = uri::split (R"(k-0+:???/)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "k-0+");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, R"(??/)");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0032) {
  auto const x = uri::split (R"(y://%DD@253.216.255.251//aa/??/://;)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "y");
  EXPECT_EQ (x->authority.userinfo, "%DD");
  EXPECT_EQ (x->authority.host, "253.216.255.251");
  EXPECT_FALSE (x->authority.port);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("", "aa", ""));
  EXPECT_EQ (x->query, "?/://;");
  EXPECT_FALSE (x->fragment);
  EXPECT_EQ (static_cast<std::string> (x->path), "//aa/");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/aa/");
}
// NOLINTNEXTLINE
TEST (Uri, 0033) {
  auto const x = uri::split ("B://.@[AC::1:6DEb:14.97.229.249]:?/#??~(");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "B");
  EXPECT_EQ (x->authority.userinfo, ".");
  EXPECT_EQ (x->authority.host, "[AC::1:6DEb:14.97.229.249]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_EQ (x->fragment, "??~(");
}
// NOLINTNEXTLINE
TEST (Uri, 0034) {
  auto const x = uri::split ("p://@26.254.86.252://aa");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "p");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "26.254.86.252");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("", "aa"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
  EXPECT_EQ (static_cast<std::string> (x->path), "//aa");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/aa");
}
// NOLINTNEXTLINE
TEST (Uri, 0035) {
  auto const x = uri::split ("P+-n:#/%f0");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P+-n");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "/%f0");
}
// NOLINTNEXTLINE
TEST (Uri, 0036) {
  auto const x = uri::split ("u:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "u");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0037) {
  auto const x = uri::split ("U://%Aa:@[::b:E:A:53.48.69.41]?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "U");
  EXPECT_EQ (x->authority.userinfo, "%Aa:");
  EXPECT_EQ (x->authority.host, "[::b:E:A:53.48.69.41]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0038) {
  auto const x = uri::split ("h.P+9:?:#?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "h.P+9");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, ":");
  EXPECT_EQ (x->fragment, "?");
}
// NOLINTNEXTLINE
TEST (Uri, 0039) {
  auto const x = uri::split ("x:??#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "x");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0040) {
  auto const x = uri::split ("A:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "A");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0041) {
  auto const x = uri::split ("Lp.:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Lp.");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0042) {
  auto const x = uri::split ("d-:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "d-");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0043) {
  auto const x = uri::split ("h-.:?/?/#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "h-.");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "/?/");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0044) {
  auto const x = uri::split ("d:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "d");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0045) {
  auto const x = uri::split ("L:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0046) {
  auto const x = uri::split ("Z5://@[9:BB:8:DAc:BbAA:E:a::]?#@$");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Z5");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "[9:BB:8:DAc:BbAA:E:a::]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "@$");
}
// NOLINTNEXTLINE
TEST (Uri, 0047) {
  auto const x = uri::split ("C-://[::1E:BB:a:5c1:Dd:40.44.228.108]/;?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "C-");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[::1E:BB:a:5c1:Dd:40.44.228.108]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (";"));
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0048) {
  auto const x = uri::split ("z://[c:BC:b:A:Bd:D:dC1f:cedB]?/#/:/%FA");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "z");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[c:BC:b:A:Bd:D:dC1f:cedB]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_EQ (x->fragment, "/:/%FA");
}
// NOLINTNEXTLINE
TEST (Uri, 0049) {
  auto const x = uri::split ("x.2:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "x.2");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0050) {
  auto const x = uri::split ("p://@[::F:e:4b:eCBE:f:c]");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "p");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "[::F:e:4b:eCBE:f:c]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0051) {
  auto const x = uri::split ("tmi://[e:C:Aa:eD::FDfD:b:F]:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "tmi");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[e:C:Aa:eD::FDfD:b:F]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0052) {
  auto const x = uri::split ("G+:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "G+");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0053) {
  auto const x = uri::split ("A://[vA5.+:=.p~=)=&_;-=7)(.;]:768295/+");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "A");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[vA5.+:=.p~=)=&_;-=7)(.;]");
  EXPECT_EQ (x->authority.port, "768295");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("+"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0054) {
  auto const x = uri::split ("n+://[::]:9831#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "n+");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[::]");
  EXPECT_EQ (x->authority.port, "9831");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0055) {
  auto const x = uri::split ("v-2e.l:?:????:/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v-2e.l");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, ":????:/");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0056) {
  auto const x = uri::split ("ka+://6.@[F::219.226.254.253]:900/'R#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "ka+");
  EXPECT_EQ (x->authority.userinfo, "6.");
  EXPECT_EQ (x->authority.host, "[F::219.226.254.253]");
  EXPECT_EQ (x->authority.port, "900");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("'R"));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0057) {
  auto const x = uri::split ("P://[daf::B:7:e:b:D:F]:730");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "P");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[daf::B:7:e:b:D:F]");
  EXPECT_EQ (x->authority.port, "730");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0058) {
  auto const x = uri::split ("H://-!:_%Bd@[::]:7");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "H");
  EXPECT_EQ (x->authority.userinfo, "-!:_%Bd");
  EXPECT_EQ (x->authority.host, "[::]");
  EXPECT_EQ (x->authority.port, "7");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0059) {
  auto const x = uri::split ("u+://;@[::dFC:d:6:d]://#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "u+");
  EXPECT_EQ (x->authority.userinfo, ";");
  EXPECT_EQ (x->authority.host, "[::dFC:d:6:d]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("", ""));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
  EXPECT_EQ (static_cast<std::string> (x->path), "//");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/");
}
// NOLINTNEXTLINE
TEST (Uri, 0060) {
  auto const x = uri::split ("D://[dCDa:c:e:B:F::D:a]:/%Dc");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "D");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[dCDa:c:e:B:F::D:a]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("%Dc"));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0061) {
  auto const x = uri::split ("mF2:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "mF2");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0062) {
  auto const x = uri::split ("f.://[d1b:CF:AbBa::F:d:11.246.155.253]?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "f.");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[d1b:CF:AbBa::F:d:11.246.155.253]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0063) {
  auto const x = uri::split ("f5++://@[7d::6:df:f:245.95.78.9]:??");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "f5++");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "[7d::6:df:f:245.95.78.9]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0064) {
  auto const x = uri::split ("c.l://[::bba:B:6:1.255.161.3]:#?/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "c.l");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[::bba:B:6:1.255.161.3]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "?/");
}
// NOLINTNEXTLINE
TEST (Uri, 0065) {
  auto const x = uri::split ("T://[fdF::f2]");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "T");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[fdF::f2]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0066) {
  auto const x = uri::split ("U-92.://[::A:C:c]/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "U-92.");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[::A:C:c]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
  EXPECT_EQ (static_cast<std::string> (x->path), "/");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/");
}
// NOLINTNEXTLINE
TEST (Uri, 0067) {
  auto const x = uri::split ("K:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "K");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0068) {
  auto const x = uri::split ("l.://[c:CEa:cd1B:f:f:D::ef]?#%bC@/:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "l.");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[c:CEa:cd1B:f:f:D::ef]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "%bC@/:");
}
// NOLINTNEXTLINE
TEST (Uri, 0069) {
  auto const x =
    uri::split (R"(v+://@[::C:dEd:4:218.255.251.5]:8/@.;J??Q??%48/#)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v+");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "[::C:dEd:4:218.255.251.5]");
  EXPECT_EQ (x->authority.port, "8");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("@.;J"));
  EXPECT_EQ (x->query, R"(?Q??%48/)");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0070) {
  auto const x = uri::split ("I:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "I");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0071) {
  auto const x = uri::split ("t.+://[::Ec:AcA:9a]:92/%8a/#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "t.+");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[::Ec:AcA:9a]");
  EXPECT_EQ (x->authority.port, "92");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("%8a", ""));
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
  EXPECT_EQ (static_cast<std::string> (x->path), "/%8a/");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/%8a/");
}
// NOLINTNEXTLINE
TEST (Uri, 0072) {
  auto const x = uri::split ("N+:?~");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "N+");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "~");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0073) {
  auto const x = uri::split ("B:?/.#?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "B");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "/.");
  EXPECT_EQ (x->fragment, "?");
}
// NOLINTNEXTLINE
TEST (Uri, 0074) {
  auto const x = uri::split ("u8K.://.(@[d::Baa:dE:D]#/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "u8K.");
  EXPECT_EQ (x->authority.userinfo, ".(");
  EXPECT_EQ (x->authority.host, "[d::Baa:dE:D]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "/");
}
// NOLINTNEXTLINE
TEST (Uri, 0075) {
  auto const x = uri::split ("E+.://@[::F:ab79:B:fa:C]#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "E+.");
  EXPECT_EQ (x->authority.userinfo, "");
  EXPECT_EQ (x->authority.host, "[::F:ab79:B:fa:C]");
  EXPECT_FALSE (x->authority.port);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0076) {
  auto const x = uri::split ("S+://[::BBc:d0:EA:3.67.149.137]:/?#/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "S+");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[::BBc:d0:EA:3.67.149.137]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "/");

  EXPECT_EQ (static_cast<std::string> (x->path), "/");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/");
}
// NOLINTNEXTLINE
TEST (Uri, 0077) {
  auto const x = uri::split (R"(Y://[4Bbc:bb::cDcd:5:c4:e:B1]:/%CA@/./??)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Y");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[4Bbc:bb::cDcd:5:c4:e:B1]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("%CA@", ".", ""));
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);

  EXPECT_EQ (static_cast<std::string> (x->path), "/%CA@/./");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/%CA@/./");
}
// NOLINTNEXTLINE
TEST (Uri, 0078) {
  auto const x = uri::split ("W.-://[CF::]://!?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "W.-");
  EXPECT_FALSE (x->authority.userinfo);
  EXPECT_EQ (x->authority.host, "[CF::]");
  EXPECT_EQ (x->authority.port, "");
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("", "!"));
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);

  EXPECT_EQ (static_cast<std::string> (x->path), "//!");
  EXPECT_EQ (static_cast<std::filesystem::path> (x->path), "/!");
}
// NOLINTNEXTLINE
TEST (Uri, 0079) {
  auto const x = uri::split ("SF6:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "SF6");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0080) {
  auto const x = uri::split (R"(R:?????////???/////#??@?_:?)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "R");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, R"(????////???/////)");
  EXPECT_EQ (x->fragment, "??@?_:?");
}
// NOLINTNEXTLINE
TEST (Uri, 0081) {
  auto const x = uri::split ("g:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "g");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0082) {
  auto const x = uri::split (R"(D-ir+.PA:??#)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "D-ir+.PA");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0083) {
  auto const x = uri::split ("Z-.-:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Z-.-");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0084) {
  auto const x = uri::split ("y-:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "y-");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0085) {
  auto const x = uri::split ("p:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "p");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0086) {
  auto const x = uri::split ("M:#*.");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "M");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "*.");
}
// NOLINTNEXTLINE
TEST (Uri, 0087) {
  auto const x = uri::split ("I:?%ab#/.");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "I");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "%ab");
  EXPECT_EQ (x->fragment, "/.");
}
// NOLINTNEXTLINE
TEST (Uri, 0088) {
  auto const x = uri::split ("v6:#:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "v6");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, ":?");
}
// NOLINTNEXTLINE
TEST (Uri, 0089) {
  auto const x = uri::split ("D:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "D");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0090) {
  auto const x = uri::split ("e.:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "e.");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0091) {
  auto const x = uri::split ("L:?#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0092) {
  auto const x = uri::split ("g-:#");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "g-");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0093) {
  auto const x = uri::split ("H:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "H");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0094) {
  auto const x = uri::split (R"(K:??)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "K");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "?");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0095) {
  auto const x = uri::split (R"(c-:?#)");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "c-");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_EQ (x->fragment, "");
}
// NOLINTNEXTLINE
TEST (Uri, 0096) {
  auto const x = uri::split ("Bw:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "Bw");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0097) {
  auto const x = uri::split ("hC:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "hC");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0098) {
  auto const x = uri::split ("q:?/#/");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "q");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "/");
  EXPECT_EQ (x->fragment, "/");
}
// NOLINTNEXTLINE
TEST (Uri, 0099) {
  auto const x = uri::split ("L:");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "L");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_FALSE (x->query);
  EXPECT_FALSE (x->fragment);
}
// NOLINTNEXTLINE
TEST (Uri, 0100) {
  auto const x = uri::split ("W-:?");
  ASSERT_TRUE (x);
  EXPECT_EQ (x->scheme, "W-");
  EXPECT_FALSE (x->authority);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_TRUE (x->path.segments.empty ());
  EXPECT_EQ (x->query, "");
  EXPECT_FALSE (x->fragment);
}

// NOLINTNEXTLINE
TEST (RemoveDotSegments, LeadingDotDotSlash) {
  auto x = uri::split ("../bar");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("..", "bar"));
  x->path.remove_dot_segments ();
  EXPECT_THAT (x->path.segments, ElementsAre ("bar"));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, LeadingDotSlash) {
  auto x = uri::split ("./bar");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (".", "bar"));
  x->path.remove_dot_segments ();
  EXPECT_THAT (x->path.segments, ElementsAre ("bar"));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, LeadingDotDotSlashDotSlash) {
  auto x = uri::split (".././bar");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("..", ".", "bar"));
  x->path.remove_dot_segments ();
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("bar"));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, MidDot) {
  auto x = uri::split ("/foo/./bar");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("foo", ".", "bar"));
  x->path.remove_dot_segments ();
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("foo", "bar"));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, LonelySlashDot) {
  auto x = uri::split ("/.");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("."));

  x->path.remove_dot_segments ();

  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, TrailingDotSlash) {
  auto x = uri::split ("/bar/./");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("bar", ".", ""));

  x->path.remove_dot_segments ();

  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("bar", ""));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, MidSlashDotDot) {
  auto x = uri::split ("/foo/../bar");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("foo", "..", "bar"));

  x->path.remove_dot_segments ();

  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("bar"));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, TrailingDotDotSlash) {
  auto x = uri::split ("/bar/../");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("bar", "..", ""));

  x->path.remove_dot_segments ();

  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, LonelySlashDotDot) {
  auto x = uri::split ("/..");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (".."));

  x->path.remove_dot_segments ();

  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));

  auto const x2 = uri::split ("/");
  EXPECT_EQ (x, x2);
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, TrailingSlashDotDot) {
  auto x = uri::split ("/bar/..");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("bar", ".."));

  x->path.remove_dot_segments ();

  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));

  auto const x2 = uri::split ("/");
  EXPECT_EQ (x, x2);
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, TwoDirectoriesTrailingSlashDotDot) {
  auto x = uri::split ("/foo/bar/..");
  ASSERT_TRUE (x);
  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("foo", "bar", ".."));

  x->path.remove_dot_segments ();

  EXPECT_TRUE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("foo", ""));

  auto const x2 = uri::split ("/foo/");
  EXPECT_EQ (x, x2);
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, LonelyDot) {
  auto x = uri::split (".");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("."));

  x->path.remove_dot_segments ();

  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, LonelyDotDot) {
  auto x = uri::split ("..");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (".."));

  x->path.remove_dot_segments ();

  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
}
// NOLINTNEXTLINE
TEST (RemoveDotSegments, LonelyDotDotSlashDot) {
  auto x = uri::split ("../.");
  ASSERT_TRUE (x);
  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre ("..", "."));

  x->path.remove_dot_segments ();

  EXPECT_FALSE (x->path.absolute);
  EXPECT_THAT (x->path.segments, ElementsAre (""));
}

// NOLINTNEXTLINE
TEST (UriFileSystemPath, Root) {
  uri::path p;
  p.absolute = true;
  EXPECT_EQ (static_cast<std::filesystem::path> (p),
             std::filesystem::path ("/"));
}
// NOLINTNEXTLINE
TEST (UriFileSystemPath, RootFoo) {
  uri::path p;
  p.absolute = true;
  p.segments.emplace_back ("foo");
  EXPECT_EQ (static_cast<std::filesystem::path> (p),
             std::filesystem::path ("/foo"));
}
// NOLINTNEXTLINE
TEST (UriFileSystemPath, AbsoluteTwoSegments) {
  uri::path p;
  p.absolute = true;
  p.segments.emplace_back ("foo");
  p.segments.emplace_back ("bar");
  EXPECT_EQ (static_cast<std::filesystem::path> (p),
             std::filesystem::path ("/foo/bar"));
}
// NOLINTNEXTLINE
TEST (UriFileSystemPath, AbsoluteTwoSegmentsDirectory) {
  uri::path p;
  p.absolute = true;
  p.segments.emplace_back ("foo");
  p.segments.emplace_back ("bar");
  p.segments.emplace_back ();
  EXPECT_EQ (static_cast<std::filesystem::path> (p),
             std::filesystem::path ("/foo/bar/"));
}
// NOLINTNEXTLINE
TEST (UriFileSystemPath, RelativeTwoSegments) {
  uri::path p;
  p.segments.emplace_back ("foo");
  p.segments.emplace_back ("bar");
  EXPECT_EQ (static_cast<std::filesystem::path> (p),
             std::filesystem::path ("foo/bar"));
}
// NOLINTNEXTLINE
TEST (UriFileSystemPath, RelativeTwoSegmentsDirectory) {
  uri::path p;
  p.segments.emplace_back ("foo");
  p.segments.emplace_back ("bar");
  p.segments.emplace_back ();
  EXPECT_EQ (static_cast<std::filesystem::path> (p),
             std::filesystem::path ("foo/bar/"));
}

class Join : public testing::Test {
protected:
  static constexpr std::string_view base_ = "http://a/b/c/d;p?q";
};

// uri::join() test cases from RFC 3986 5.4.1. Normal Examples.
// NOLINTNEXTLINE
TEST_F (Join, Normal) {
  EXPECT_EQ (uri::split ("g:h"), uri::join (base_, "g:h"));
  EXPECT_EQ (uri::split ("http://a/b/c/g"), uri::join (base_, "g"));
  EXPECT_EQ (uri::split ("http://a/b/c/g"), uri::join (base_, "./g"));
  EXPECT_EQ (uri::split ("http://a/b/c/g/"), uri::join (base_, "g/"));
  EXPECT_EQ (uri::split ("http://a/g"), uri::join (base_, "/g"));
  EXPECT_EQ (uri::split ("http://g"), uri::join (base_, "//g"));
  EXPECT_EQ (uri::split ("http://a/b/c/d;p?y"), uri::join (base_, "?y"));
  EXPECT_EQ (uri::split ("http://a/b/c/g?y"), uri::join (base_, "g?y"));
  EXPECT_EQ (uri::split ("http://a/b/c/d;p?q#s"), uri::join (base_, "#s"));
  EXPECT_EQ (uri::split ("http://a/b/c/d;p?q#s"), uri::join (base_, "#s"));
  EXPECT_EQ (uri::split ("http://a/b/c/g#s"), uri::join (base_, "g#s"));
  EXPECT_EQ (uri::split ("http://a/b/c/g?y#s"), uri::join (base_, "g?y#s"));
  EXPECT_EQ (uri::split ("http://a/b/c/;x"), uri::join (base_, ";x"));
  EXPECT_EQ (uri::split ("http://a/b/c/g;x"), uri::join (base_, "g;x"));
  EXPECT_EQ (uri::split ("http://a/b/c/g;x?y#s"), uri::join (base_, "g;x?y#s"));
  EXPECT_EQ (uri::split ("http://a/b/c/d;p?q"), uri::join (base_, ""));
  EXPECT_EQ (uri::split ("http://a/b/c/"), uri::join (base_, "."));
  EXPECT_EQ (uri::split ("http://a/b/c/"), uri::join (base_, "./"));
  EXPECT_EQ (uri::split ("http://a/b/"), uri::join (base_, ".."));
  EXPECT_EQ (uri::split ("http://a/b/"), uri::join (base_, "../"));
  EXPECT_EQ (uri::split ("http://a/b/g"), uri::join (base_, "../g"));
  EXPECT_EQ (uri::split ("http://a/"), uri::join (base_, "../.."sv));
  EXPECT_EQ (uri::split ("http://a/"), uri::join (base_, "../../"));
  EXPECT_EQ (uri::split ("http://a/g"), uri::join (base_, "../../g"));
  EXPECT_EQ (uri::split ("file://user@a/g"), uri::join ("file://user@a", "../../g"));
}

// NOLINTNEXTLINE
TEST_F (Join, Abnormal) {
  // Check that we are careful in handling cases where there are more ".."
  // segments in a relative-path reference than there are hierarchical levels in
  // the base URI's path.
  EXPECT_EQ (uri::split ("http://a/g"), uri::join (base_, "../../../g"));
  EXPECT_EQ (uri::split ("http://a/g"), uri::join (base_, "../../../../g"));

  // Check we correctly remove the dot-segments "." and ".." when they are
  // complete components of a path, but not when they are only part of a
  // segment.
  EXPECT_EQ (uri::split ("http://a/g"), uri::join (base_, "/./g"));
  EXPECT_EQ (uri::split ("http://a/g"), uri::join (base_, "/../g"));
  EXPECT_EQ (uri::split ("http://a/b/c/g."), uri::join (base_, "g."));
  EXPECT_EQ (uri::split ("http://a/b/c/.g"), uri::join (base_, ".g"));
  EXPECT_EQ (uri::split ("http://a/b/c/g.."), uri::join (base_, "g.."));
  EXPECT_EQ (uri::split ("http://a/b/c/..g"), uri::join (base_, "..g"));

  // Verify cases where the relative reference uses unnecessary or nonsensical
  // forms of the "." and ".." complete path segments.
  EXPECT_EQ (uri::split ("http://a/b/g"), uri::join (base_, "./../g"));
  EXPECT_EQ (uri::split ("http://a/b/c/g/"), uri::join (base_, "./g/."));
  EXPECT_EQ (uri::split ("http://a/b/c/g/h"), uri::join (base_, "g/./h"));
  EXPECT_EQ (uri::split ("http://a/b/c/h"), uri::join (base_, "g/../h"));
  EXPECT_EQ (uri::split ("http://a/b/c/g;x=1/y"),
             uri::join (base_, "g;x=1/./y"));
  EXPECT_EQ (uri::split ("http://a/b/c/y"), uri::join (base_, "g;x=1/../y"));

  // Check that we correctly separate the reference's query and/or fragment
  // components from the path component before merging it with the base path and
  // removing dot-segments.
  EXPECT_EQ (uri::split ("http://a/b/c/g?y/./x"), uri::join (base_, "g?y/./x"));
  EXPECT_EQ (uri::split ("http://a/b/c/g?y/../x"),
             uri::join (base_, "g?y/../x"));
  EXPECT_EQ (uri::split ("http://a/b/c/g#s/./x"), uri::join (base_, "g#s/./x"));
  EXPECT_EQ (uri::split ("http://a/b/c/g#s/../x"),
             uri::join (base_, "g#s/../x"));

  // Verify the behviour when the scheme name is present in a relative reference
  // if it is the same as the base URI scheme.
  EXPECT_EQ (uri::split ("http:g"), uri::join (base_, "http:g"));
}

// NOLINTNEXTLINE
TEST (UriCompose, Empty) {
  uri::parts p;
  EXPECT_EQ (uri::compose (p), "");
}
// NOLINTNEXTLINE
TEST (UriCompose, Scheme) {
  uri::parts p;
  p.scheme = "file";
  EXPECT_EQ (uri::compose (p), "file:");
}
// NOLINTNEXTLINE
TEST (UriCompose, Authority) {
  uri::parts p;
  p.authority.userinfo = "username";
  p.authority.host = "host";
  p.authority.port = "123";
  auto const expected = "//username@host:123"sv;
  EXPECT_EQ (uri::compose (p), expected);
  EXPECT_EQ (uri::split (expected), p);
}
// NOLINTNEXTLINE
TEST (UriCompose, AbsolutePath) {
  uri::parts p;
  p.path.absolute = true;
  p.path.segments.emplace_back ("a");
  p.path.segments.emplace_back ("b");
  p.path.segments.emplace_back ();
  auto const expected = "/a/b/"sv;
  EXPECT_EQ (uri::compose (p), expected);
  EXPECT_EQ (uri::split (expected), p);
}
// NOLINTNEXTLINE
TEST (UriCompose, RelativePath) {
  uri::parts p;
  p.path.segments.emplace_back ("a");
  p.path.segments.emplace_back ("b");
  p.path.segments.emplace_back ();
  auto const expected = "a/b/"sv;
  EXPECT_EQ (uri::compose (p), expected);
  EXPECT_EQ (uri::split (expected), p);
}
// NOLINTNEXTLINE
TEST (UriCompose, Query) {
  uri::parts p;
  p.query = "query";
  auto const expected = "?query"sv;
  EXPECT_EQ (uri::compose (p), expected);
  EXPECT_EQ (uri::split (expected), p);
}
// NOLINTNEXTLINE
TEST (UriCompose, Fragment) {
  uri::parts p;
  p.fragment = "fragment";
  auto const expected = "#fragment"sv;
  EXPECT_EQ (uri::compose (p), expected);
  EXPECT_EQ (uri::split (expected), p);
}
