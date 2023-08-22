#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

#include "uri.hpp"

using namespace std::literals::string_view_literals;
using namespace std::literals::string_literals;

namespace {

#if 0
bool starts_with (std::string_view in, std::string_view s) {
  return in.size () >= s.size () && in.compare (0, s.size (), s) == 0;
  // return in.starts_with (s);
}
#endif

// The following normalizations are described in RFC 3986 [1] to result in
// equivalent URIs:
//
//  Converting percent-encoded triplets to uppercase. The hexadecimal digits
//  within a percent- encoding triplet of the URI (e.g., %3a versus %3A) are
//  case-insensitive and therefore should be normalized to use uppercase letters
//  for the digits A-F.[2] Example:
//
//     http://example.com/foo%2a → http://example.com/foo%2A
//
//  Converting the scheme and host to lowercase. The scheme and host components
//  of the URI are case- insensitive and therefore should be normalized to
//  lowercase.[3] Example:
//
//     HTTP://User@Example.COM/Foo → http://User@example.com/Foo
//
//  Decoding percent-encoded triplets of unreserved characters. Percent-encoded
//  triplets of the URI in the ranges of ALPHA (%41–%5A and %61–%7A), DIGIT
//  (%30–%39), hyphen (%2D), period (%2E), underscore (%5F), or tilde (%7E) do
//  not require percent-encoding and should be decoded to their corresponding
//  unreserved characters.[4] Example:
//
//    http://example.com/%7Efoo → http://example.com/~foo
//
//  Removing dot-segments. Dot-segments . and .. in the path component of the
//  URI should be removed by applying the remove_dot_segments algorithm[5] to
//  the path described in RFC 3986.[6] Example:
//
//    http://example.com/foo/./bar/baz/../qux → http://example.com/foo/bar/qux
//
//  Converting an empty path to a "/" path. In presence of an authority
//  component, an empty path component should be normalized to a path component
//  of "/".[7] Example:
//
//    http://example.com → http://example.com/
//
//  Removing the default port. An empty or default port component of the URI
//  (port 80 for the http scheme) with its ":" delimiter should be removed.[8]
//  Example:
//
//    http://example.com:80/ → http://example.com/
//
//  [1]: RFC 3986, Section 6. Normalization and Comparison
//  [2]: RFC 3986, Section 6.2.2.1. Case Normalization
//  [3]: RFC 3986, Section 6.2.2.1. Case Normalization
//  [4]: RFC 3986, Section 6.2.2.3. Path Segment Normalization
//  [5]: RFC 3986, 5.2.4. Remove Dot Segments
//  [6]: RFC 3986, 6.2.2.3. Path Segment Normalization
//  [7]: RFC 3986, Section 6.2.3. Scheme-Based Normalization
//  [8]: RFC 3986, Section 6.2.3. Scheme-Based Normalization

#if 0
std::string percent_decode (std::string_view src) {
  std::string result;
  result.reserve (src.length ());
  for (auto pos = src.begin (), end = src.end (); pos != end; ++pos) {
    if (*pos == '%' && std::distance (pos, end) >= 3 &&
        std::isxdigit (*(pos + 1)) && std::isxdigit (*(pos + 2))) {
      auto hex2dec = [] (char c) {
        return static_cast<unsigned> (c) -
               static_cast<unsigned> (std::toupper (c) >= 'A' ? 'A' + 10 : '0');
      };
      result += static_cast<char> ((hex2dec (*(pos + 1)) << 4) |
                                   hex2dec (*(pos + 2)));
      pos += 2;
    } else {
      result += *pos;
    }
  }
  return result;
}
#endif

#if 0
std::vector<std::string> remove_dot_segments (std::string_view in) {
  std::vector<std::string> out;

  // 1.  The input buffer is initialized with the now-appended path
  //       components and the output buffer is initialized to the empty
  //       string.
  //
  //   2.  While the input buffer is not empty, loop as follows:
  //
  //       A.  If the input buffer begins with a prefix of "../" or "./",
  //           then remove that prefix from the input buffer; otherwise,
  //
  //       B.  if the input buffer begins with a prefix of "/./" or "/.",
  //           where "." is a complete path segment, then replace that
  //           prefix with "/" in the input buffer; otherwise,
  //
  //       C.  if the input buffer begins with a prefix of "/../" or "/..",
  //           where ".." is a complete path segment, then replace that
  //           prefix with "/" in the input buffer and remove the last
  //           segment and its preceding "/" (if any) from the output
  //           buffer; otherwise,
  //
  //       D.  if the input buffer consists only of "." or "..", then remove
  //           that from the input buffer; otherwise,
  //
  //       E.  move the first path segment in the input buffer to the end of
  //           the output buffer, including the initial "/" character (if
  //           any) and any subsequent characters up to, but not including,
  //           the next "/" character or the end of the input buffer.
  //
  //   3.  Finally, the output buffer is returned as the result of
  //       remove_dot_segments.

  if (starts_with (in, "/")) {
    in.remove_prefix (1);
  }
  while (!in.empty ()) {
    if (starts_with (in, "../")) {
      in.remove_prefix (3);
    } else if (starts_with (in, "./")) {
      in.remove_prefix (2);
    } else if (starts_with (in, "/./")) {
      in.remove_prefix (3);
    } else if (in == "/.") {
      in.remove_prefix (2);
    } else if (starts_with (in, "/../")) {
      in.remove_prefix (4);
      if (!out.empty ()) {
        out.pop_back ();
      }
    } else if (in == "/..") {
      in.remove_prefix (3);
      if (!out.empty ()) {
        out.pop_back ();
      }
    } else if (in == ".") {
      in.remove_prefix (1);
    } else if (in == "..") {
      in.remove_prefix (2);
    } else {
      auto const pos = in.find ('/');
      if (pos == 0) {
        in.remove_prefix (1);
      } else {
        out.emplace_back (percent_decode (in.substr (0, pos)));
        in.remove_prefix (pos != std::string::npos ? pos : in.length ());
      }
    }
  }
  return out;
}
#endif

bool read_stream (std::istream& is) {
  std::string line;
  while (getline (is, line)) {
    std::cout << "URI: " << line << '\n';

    auto r = uri_rule (line);
    if (!r) {
      return false;
    }
    auto value_or_none = [] (std::optional<std::string> const& s) {
      return s ? "\""s + s.value () + "\""s : "None"s;
    };
    std::cout << " scheme: " << value_or_none (r->scheme) << '\n'
              << " userinfo: " << value_or_none (r->userinfo) << '\n'
              << " host: " << value_or_none (r->host) << '\n'
              << " port: " << value_or_none (r->port) << '\n';
    std::string path;
    for (auto const& s : r->segments) {
      path += s;
    }
    std::cout << " path: " << std::quoted (path) << '\n';
    std::cout << " query: " << value_or_none (r->query) << '\n';
    std::cout << " fragment: " << value_or_none (r->fragment) << '\n';
  }
  return true;
}

}  // end anonymous namespace

int main (int argc, char const* argv[]) {
  int exit_code = EXIT_SUCCESS;
  if (argc == 1) {
    // read_stream (std::cin);
    std::istringstream in{"Z://-@[b8::C:AB:2b]:16?%FC:"};
    if (!read_stream (in)) {
      exit_code = EXIT_FAILURE;
    }
  } else {
    for (int arg = 1; arg < argc; ++arg) {
      std::filesystem::path const p = argv[arg];
      std::ifstream infile{p};
      if (!infile.is_open ()) {
        std::cerr << "Error: couldn't open " << p << '\n';
        exit_code = EXIT_FAILURE;
      } else {
        read_stream (infile);
      }
    }
  }
  return exit_code;
}

#if 0
  std::vector<std::string> out = remove_dot_segments (uri);
  std::string result;
  for (auto const & s: out) {
    result += '/';
    result += s;
  }
  std::cout << std::quoted(result) << '\n';
#endif
