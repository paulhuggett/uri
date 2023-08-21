#ifndef URI_HPP
#define URI_HPP

#include "rule.hpp"

// URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
//
// hier-part     = "//" authority path-abempty
//               / path-absolute
//               / path-rootless
//               / path-empty
//
// URI-reference = URI / relative-ref
//
// absolute-URI  = scheme ":" hier-part [ "?" query ]
//
// relative-ref  = relative-part [ "?" query ] [ "#" fragment ]
//
// relative-part = "//" authority path-abempty
//               / path-absolute
//               / path-noscheme
//               / path-empty
//
// scheme        = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
//
// authority     = [ userinfo "@" ] host [ ":" port ]
// userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
// host          = IP-literal / IPv4address / reg-name
// port          = *DIGIT
//
// IP-literal    = "[" ( IPv6address / IPvFuture  ) "]"
//
// IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
//
// IPv6address   =                            6( h16 ":" ) ls32
//               /                       "::" 5( h16 ":" ) ls32
//               / [               h16 ] "::" 4( h16 ":" ) ls32
//               / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
//               / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
//               / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32
//               / [ *4( h16 ":" ) h16 ] "::"              ls32
//               / [ *5( h16 ":" ) h16 ] "::"              h16
//               / [ *6( h16 ":" ) h16 ] "::"
//
// h16           = 1*4HEXDIG
// ls32          = ( h16 ":" h16 ) / IPv4address
// IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
// dec-octet     = DIGIT                 ; 0-9
//               / %x31-39 DIGIT         ; 10-99
//               / "1" 2DIGIT            ; 100-199
//               / "2" %x30-34 DIGIT     ; 200-249
//               / "25" %x30-35          ; 250-255
//
// reg-name      = *( unreserved / pct-encoded / sub-delims )
//
// path          = path-abempty    ; begins with "/" or is empty
//               / path-absolute   ; begins with "/" but not "//"
//               / path-noscheme   ; begins with a non-colon segment
//               / path-rootless   ; begins with a segment
//               / path-empty      ; zero characters
//
// path-abempty  = *( "/" segment )
// path-absolute = "/" [ segment-nz *( "/" segment ) ]
// path-noscheme = segment-nz-nc *( "/" segment )
// path-rootless = segment-nz *( "/" segment )
// path-empty    = 0<pchar>
//
// segment       = *pchar
// segment-nz    = 1*pchar
// segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
//                  ; non-zero-length segment without any colon ":"
//
// pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
//
// query         = *( pchar / "/" / "?" )
//
// fragment      = *( pchar / "/" / "?" )
//
// pct-encoded   = "%" HEXDIG HEXDIG
//
// unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
// reserved      = gen-delims / sub-delims
// gen-delims    = ":" / "/" / "?" / "#" / "[" / "]" / "@"
// sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
//               / "*" / "+" / "," / ";" / "="

struct uri_parts {
  void clear () {
    scheme.reset ();
    userinfo.reset ();
    host.reset ();
    port.reset ();
    segments.clear ();
    query.reset ();
    fragment.reset ();
  }

  std::optional<std::string> scheme;
  std::optional<std::string> userinfo;
  std::optional<std::string> host;
  std::optional<std::string> port;
  std::vector<std::string> segments;
  std::optional<std::string> query;
  std::optional<std::string> fragment;
};

class uri {
  static auto single_colon (rule&& r) {
    return r.concat (colon)
      .concat ([] (rule&& r1) -> rule::matched_result {
        if (auto const& sv = r1.tail ()) {
          if (sv->empty () || sv->front () != ':') {
            return std::make_tuple (sv->substr (0, 0),
                                    rule::acceptor_container{});
          }
        }
        return {};
      })
      .matched ("single-colon", r);
  }

  // sub-delims    = "!" / "$" / "&" / "'" / "(" / ")"
  //               / "*" / "+" / "," / ";" / "="
  static auto sub_delims (rule&& r) {
    return r.single_char ([] (char const c) {
      return c == '!' || c == '$' || c == '&' || c == '\'' || c == '(' ||
             c == ')' || c == '*' || c == '+' || c == ',' || c == ';' ||
             c == '=';
    });
  }
  // unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
  static auto unreserved (rule&& r) {
    return r.single_char ([] (char const c) {
      return std::isalnum (static_cast<int> (c)) || c == '-' || c == '.' ||
             c == '_' || c == '~';
    });
  }
  // pct-encoded   = "%" HEXDIG HEXDIG
  static auto pct_encoded (rule&& r) {
    return r.concat (single_char ('%'))
      .concat (hexdig)
      .concat (hexdig)
      .matched ("pct-encoded", r);
  }
  // pchar         = unreserved / pct-encoded / sub-delims / ":" / "@"
  static auto pchar (rule&& r) {
    return r
      .alternative (unreserved, pct_encoded, sub_delims, colon, commercial_at)
      .matched ("pchar", r);
  }
  // userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
  static auto userinfo (rule&& r) {
    return r
      .star ([] (rule&& r2) {
        return r2.alternative (unreserved, pct_encoded, sub_delims, colon)
          .matched ("userinfo/*", r2);
      })
      .matched ("userinfo", r);
  }
  // scheme = ALPHA *( ALPHA / DIGIT / "+" / "-" / "." )
  static auto scheme (rule&& r) {
    return r.concat (alpha)
      .star ([] (rule&& r2) {
        return r2.alternative (alpha, digit, plus, minus, full_stop)
          .matched ("scheme/*", r2);
      })
      .matched ("scheme", r);
  }
  // reg-name      = *( unreserved / pct-encoded / sub-delims )
  static auto reg_name (rule&& r) {
    return r
      .star ([] (rule&& r1) {
        return r1
          .alternative (uri::unreserved, uri::pct_encoded, uri::sub_delims)
          .matched ("reg-name/*", r1);
      })
      .matched ("reg-name", r);
  }
  // dec-octet     = DIGIT                 ; 0-9
  //               / %x31-39 DIGIT         ; 10-99
  //               / "1" 2DIGIT            ; 100-199
  //               / "2" %x30-34 DIGIT     ; 200-249
  //               / "25" %x30-35          ; 250-255
  static auto dec_octet (rule&& r) {
    return r
      .alternative (
        [] (rule&& r4) {
          return r4                          // 250-255
            .concat (single_char ('2'))      // "2"
            .concat (single_char ('5'))      // "5"
            .concat (char_range ('0', '5'))  // %x30-35
            .matched ("\"25\" %x30-35", r4);
        },
        [] (rule&& r3) {
          return r3                          // 200-249
            .concat (single_char ('2'))      // "2"
            .concat (char_range ('0', '4'))  // %x30-34
            .concat (digit)                  // DIGIT
            .matched ("\"2\" %x30-34 DIGIT", r3);
        },
        [] (rule&& r2) {
          return r2                      // 100-199
            .concat (single_char ('1'))  // "1"
            .concat (digit)              // 2DIGIT
            .concat (digit)              // (...)
            .matched ("\"1\" 2DIGIT", r2);
        },
        [] (rule&& r1) {
          return r1                          // 10-99
            .concat (char_range ('1', '9'))  // %x31-39
            .concat (digit)                  // DIGIT
            .matched ("%x31-39 DIGIT", r1);
        },
        digit)
      .matched ("dec-octet", r);
  }
  // IPv4address   = dec-octet "." dec-octet "." dec-octet "." dec-octet
  static auto ipv4address (rule&& r) {
    return r
      .concat (uri::dec_octet)  // dec-octet
      .concat (full_stop)       // "."
      .concat (uri::dec_octet)  // dec-octet
      .concat (full_stop)       // "."
      .concat (uri::dec_octet)  // dec-octet
      .concat (full_stop)       // "."
      .concat (uri::dec_octet)  // dec-octet
      .matched ("IPv4address", r);
  }
  // h16 = 1*4HEXDIG
  static auto h16 (rule&& r) {
    return r.star (hexdig, 1, 4).matched ("h16", r);
  }
  // h16colon = h16 ":"
  static auto h16_colon (rule&& r) {
    return r.concat (h16).concat (single_colon).matched ("h16:", r);
  }
  static auto colon_colon (rule&& r) {
    return r.concat (colon).concat (colon).matched ("\"::\"", r);
  }
  // ls32          = ( h16 ":" h16 ) / IPv4address
  static auto ls32 (rule&& r) {
    return r
      .alternative (
        [] (rule&& r1) {
          return r1.concat (uri::h16).concat (colon).concat (uri::h16).matched (
            "h16:h16", r1);
        },
        uri::ipv4address)
      .matched ("ls32", r);
  }
  // IPv6address =                            6( h16 ":" ) ls32 // r1
  //             /                       "::" 5( h16 ":" ) ls32 // r2
  //             / [               h16 ] "::" 4( h16 ":" ) ls32 // r3
  //             / [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32 // r4
  //             / [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32 // r5
  //             / [ *3( h16 ":" ) h16 ] "::"    h16 ":"   ls32 // r6
  //             / [ *4( h16 ":" ) h16 ] "::"              ls32 // r7
  //             / [ *5( h16 ":" ) h16 ] "::"              h16  // r8
  //             / [ *6( h16 ":" ) h16 ] "::"                   // r9
  static auto ipv6address (rule&& r) {
    return r
      .alternative (
        [] (rule&& r1) {
          // 6( h16 ":" ) ls32
          return r1.star (uri::h16_colon, 6, 6)
            .concat (uri::ls32)
            .matched ("6( h16: ) ls32", r1);
        },
        [] (rule&& r2) {
          // "::" 5( h16 ":" ) ls32
          return r2.concat (uri::colon_colon)
            .star (uri::h16_colon, 5, 5)
            .concat (uri::ls32)
            .matched ("\"::\" 5( h16 colon ) ls32", r2);
        },
        [] (rule&& r3) {
          // [ h16 ] "::" 4( h16 ":" ) ls32
          return r3.optional (uri::h16)
            .concat (uri::colon_colon)
            .star (uri::h16_colon, 4, 4)
            .concat (uri::ls32)
            .matched ("[ h16 ] \"::\" 4( h16 colon ) ls32", r3);
        },
        [] (rule&& r4) {
          // [ *1( h16 ":" ) h16 ] "::" 3( h16 ":" ) ls32
          return r4
            .optional ([] (rule&& r4a) {
              return r4a.star (uri::h16_colon, 0, 1)
                .concat (uri::h16)
                .matched ("*1( h16 colon ) h16", r4a);
            })
            .concat (uri::colon_colon)
            .star (uri::h16_colon, 3, 3)
            .concat (uri::ls32)
            .matched ("[ *1( h16 colon ) h16 ] \"::\" 3( h16 colon ) ls32", r4);
        },
        [] (rule&& r5) {
          // [ *2( h16 ":" ) h16 ] "::" 2( h16 ":" ) ls32
          return r5
            .optional ([] (rule&& r5a) {
              return r5a.star (uri::h16_colon, 0, 2)
                .concat (uri::h16)
                .matched ("*2( h16 colon ) h16", r5a);
            })
            .concat (uri::colon_colon)
            .star (uri::h16_colon, 2, 2)
            .concat (uri::ls32)
            .matched ("[ *2( h16 colon ) h16 ] \"::\" 2( h16 colon ) ls32", r5);
        },
        [] (rule&& r6) {
          // [ *3( h16 ":" ) h16 ] "::" h16 ":" ls32
          return r6
            .optional ([] (rule&& r6a) {
              return r6a.star (uri::h16_colon, 0, 3)
                .concat (uri::h16)
                .matched ("*3( h16 colon ) h16", r6a);
            })
            .concat (uri::colon_colon)
            .concat (uri::h16_colon)
            .concat (uri::ls32)
            .matched ("[ *3( h16 colon ) h16 ] \"::\" h16 colon ls32", r6);
        },
        [] (rule&& r7) {
          // [ *4( h16 ":" ) h16 ] "::" ls32
          return r7
            .optional ([] (rule&& r7a) {
              return r7a.star (uri::h16_colon, 0, 4)
                .concat (uri::h16)
                .matched ("*4( h16 colon ) h16", r7a);
            })
            .concat (uri::colon_colon)
            .concat (uri::ls32)
            .matched ("[ *4( h16 colon ) h16 ] \"::\" ls32", r7);
        },
        [] (rule&& r8) {
          // [ *5( h16 ":" ) h16 ] "::" h16
          return r8
            .optional ([] (rule&& r8a) {
              return r8a.star (uri::h16_colon, 0, 5)
                .concat (uri::h16)
                .matched ("*5( h16 colon ) h16", r8a);
            })
            .concat (uri::colon_colon)
            .concat (uri::h16)
            .matched ("[ *5( h16 colon ) h16 ] \"::\" h16", r8);
        },
        [] (rule&& r9) {
          // [ *6( h16 ":" ) h16 ] "::"
          return r9
            .optional ([] (rule&& r9a) {
              return r9a.star (uri::h16_colon, 0, 6)
                .concat (uri::h16)
                .matched ("*6( h16 colon ) h16", r9a);
            })
            .concat (uri::colon_colon)
            .matched ("[ *6( h16 colon ) h16 ] \"::\"", r9);
        })
      .matched ("IPv6address", r);
  }
  // IPvFuture     = "v" 1*HEXDIG "." 1*( unreserved / sub-delims / ":" )
  static auto ipvfuture (rule&& r) {
    return r.concat (single_char ('v'))
      .star (hexdig, 1)
      .concat (full_stop)
      .star (
        [] (rule&& r1) {
          return r1.alternative (uri::unreserved, uri::sub_delims, colon)
            .matched ("unreserved / sub-delims / colon", r1);
        },
        1)
      .matched ("IPvFuture", r);
  }
  // IP-literal    = "[" ( IPv6address / IPvFuture ) "]"
  static auto ip_literal (rule&& r) {
    return r.concat (left_square_bracket)
      .concat ([] (rule&& r1) {
        return r1.alternative (uri::ipv6address, uri::ipvfuture)
          .matched ("IPv6address / IPvFuture", r1);
      })
      .concat (right_square_bracket)
      .matched ("IP-literal", r);
  }

  auto host_rule () {
    // host          = IP-literal / IPv4address / reg-name
    return [&result = result_] (rule&& r) {
      return r
        .concat (
          [] (rule&& r1) {
            return r1
              .alternative (uri::ip_literal, uri::ipv4address, uri::reg_name)
              .matched ("IP-literal / IPv4address / reg-name", r1);
          },
          [&result] (std::string_view host) { result.host = host; })
        .matched ("host", r);
    };
  }
  auto userinfo_at () {
    // userinfo-at = userinfo "@"
    return [&result = result_] (rule&& r) {
      return r
        .concat (uri::userinfo,
                 [&result] (std::string_view const userinfo) {
                   result.userinfo = userinfo;
                 })
        .concat (commercial_at)
        .matched ("userinfo \"@\"", r);
    };
  }

  // port = *DIGIT
  static auto port (rule&& r) { return r.star (digit).matched ("port", r); }

  auto colon_port () {
    // colon-port = ":" port
    return [&result = result_] (rule&& r) {
      return r.concat (colon)
        .concat (port,
                 [&result] (std::string_view const p) { result.port = p; })
        .matched ("\":\" port", r);
    };
  }

  auto authority () {
    // authority = [ userinfo "@" ] host [ ":" port ]
    return [this] (rule&& r) {
      return r.optional (this->userinfo_at ())
        .concat (this->host_rule ())
        .optional (this->colon_port ())
        .matched ("authority", r);
    };
  }

  // segment       = *pchar
  static auto segment (rule&& r) {
    return r.star (uri::pchar).matched ("segment", r);
  }
  // segment-nz    = 1*pchar
  static auto segment_nz (rule&& r) {
    return r.star (uri::pchar, 1U).matched ("segment-nz", r);
  }
  // segment-nz-nc = 1*( unreserved / pct-encoded / sub-delims / "@" )
  //                  ; non-zero-length segment without any colon ":"
  static auto segment_nz_nc (rule&& r) {
    return r
      .star (
        [] (rule&& r2) {
          return r2
            .alternative (uri::unreserved, uri::pct_encoded, uri::sub_delims,
                          commercial_at)
            .matched ("unreserved / pct-encoded / sub-delims / \"@\"", r2);
        },
        1U)
      .matched ("segment-nz-nc", r);
  }
  auto path_abempty () {
    // path-abempty  = *( "/" segment )
    return [&result = result_] (rule&& r) {
      return r
        .star ([&result] (rule&& r2) {
          return r2
            .concat (solidus,
                     [&result] (std::string_view const seg) {
                       result.segments.emplace_back (seg);
                     })
            .concat (segment,
                     [&result] (std::string_view const seg) {
                       result.segments.back () += seg;
                     })
            .matched ("\"/\" segment", r2);
        })
        .matched ("path-abempty", r);
    };
  }
  auto path_absolute () {
    // path-absolute = "/" [ segment-nz *( "/" segment ) ]
    return [&result = result_] (rule&& r) {
      return r
        .concat (solidus,
                 [&result] (std::string_view const seg) {
                   result.segments.emplace_back (seg);
                 })
        .optional ([&result] (rule&& r1) {
          return r1
            .concat (uri::segment_nz,
                     [&result] (std::string_view seg) {
                       result.segments.back () += seg;
                     })
            .concat ([&result] (rule&& r2) {
              return r2
                .star ([&result] (rule&& r3) {
                  return r3
                    .concat (solidus,
                             [&result] (std::string_view const seg) {
                               result.segments.emplace_back (seg);
                             })
                    .concat (segment,
                             [&result] (std::string_view const seg) {
                               result.segments.back () += seg;
                             })
                    .matched ("\"/\" segment", r3);
                })
                .matched ("*( \"/\" segment )", r2);
            })
            .matched ("*( \"/\" segment )", r1);
        })
        .matched ("path-absolute", r);
    };
  }
  // path-empty    = 0<pchar>
  static auto path_empty (rule&& r) {
    return r.star (uri::pchar, 0, 0).matched ("path-empty", r);
  }
  // path-rootless = segment-nz *( "/" segment )
  auto path_rootless () {
    return [&result = result_] (rule&& r) {
      return r
        .concat (uri::segment_nz,
                 [&result] (std::string_view const seg) {
                   result.segments.emplace_back (seg);
                 })
        .star ([&result] (rule&& r1) {
          return r1
            .concat (solidus,
                     [&result] (std::string_view const seg) {
                       result.segments.emplace_back (seg);
                     })
            .concat (segment,
                     [&result] (std::string_view const seg) {
                       result.segments.back () += seg;
                     })
            .matched ("\"/\" segment", r1);
        })
        .matched ("path-rootless", r);
    };
  }

  // hier-part     = "//" authority path-abempty
  //               / path-absolute
  //               / path-rootless
  //               / path-empty
  auto hier_part () {
    return [this] (rule&& r) {
      return r
        .alternative (
          [this] (rule&& r1) {
            return r1.concat (solidus)
              .concat (solidus)
              .concat (this->authority ())
              .concat (this->path_abempty ())
              .matched ("\"//\" authority path-abempty", r1);
          },
          this->path_absolute (), this->path_rootless (), path_empty)
        .matched ("hier-part", r);
    };
  }

  // query         = *( pchar / "/" / "?" )
  static auto query (rule&& r) {
    return r
      .star ([] (rule&& r2) {
        return r2.alternative (uri::pchar, solidus, question_mark)
          .matched (R"(pchar / "/" / "?")", r2);
      })
      .matched ("query", r);
  }
  // fragment      = *( pchar / "/" / "?" )
  static auto fragment (rule&& r) { return query (std::move (r)); }

public:
  // URI           = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
  std::optional<uri_parts> uri_rule (std::string_view const in) {
    bool success =
      rule{in}
        .concat (uri::scheme,
                 [&result = result_] (std::string_view const scheme) {
                   result.scheme = scheme;
                 })
        .concat (colon)
        .concat (this->hier_part ())
        .optional ([&result = result_] (rule&& rq) {
          // "?" query
          return rq
            .concat (question_mark)  // "?"
            .concat (uri::query,     // query
                     [&result] (std::string_view const query) {
                       result.query = query;
                     })
            .matched ("\"?\" query", rq);
        })
        .optional ([&result = result_] (rule&& rf) {
          // "#" fragment
          return rf
            .concat (hash)  // "#"
            .concat (       // fragment
              uri::fragment,
              [&result] (std::string_view const fragment) {
                result.fragment = fragment;
              })
            .matched ("\"#\" fragment", rf);
        })
        .done ();
    if (success) {
      return result_;
    }
    return {};
  }

  void clear () { result_.clear (); }
  uri_parts const& result () const { return result_; }

private:
  uri_parts result_;
};

#endif /* uri_h */
