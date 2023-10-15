//===- include/uri/uri.hpp --------------------------------*- mode: C++ -*-===//
//*             _  *
//*  _   _ _ __(_) *
//* | | | | '__| | *
//* | |_| | |  | | *
//*  \__,_|_|  |_| *
//*                *
//===----------------------------------------------------------------------===//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE for information.
// SPDX-License-Identifier: MIT
//===----------------------------------------------------------------------===//
#ifndef URI_URI_HPP
#define URI_URI_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "uri/pctdecode.hpp"
#include "uri/pctencode.hpp"

namespace uri {

enum class code_points : char32_t {
  null = 0x00,
  tab = 0x09,
  lf = 0x0A,
  cr = 0x0D,
  space = 0x20,
  number_sign = 0x23,
  solidus = 0x2F,
  exclamation_mark = 0x21,
  digit_zero = 0x30,
  digit_nine = 0x39,
  colon = 0x3A,
  less_than_sign = 0x3C,
  greater_than_sign = 0x3E,
  question_mark = 0x3F,
  commercial_at = 0x40,
  latin_capital_letter_a = 0x41,
  latin_capital_letter_z = 0x5A,
  left_square_bracket = 0x5B,
  reverse_solidus = 0x5C,
  right_square_bracket = 0x5D,
  circumflex_accent = 0x5E,
  latin_small_letter_a = 0x61,
  latin_small_letter_z = 0x7A,
  vertical_line = 0x7C,
  tilde = 0x7E,
};

struct parts {
  struct path {
    bool absolute = false;
    std::vector<std::string_view> segments;

    // Remove dot segments from the path.
    void remove_dot_segments ();
    [[nodiscard]] bool empty () const noexcept { return segments.empty (); }
    [[nodiscard]] bool valid () const noexcept;
    explicit operator std::string () const;
    explicit operator std::filesystem::path () const;
    bool operator== (path const& rhs) const;
    bool operator!= (path const& rhs) const { return !operator== (rhs); }
  };
  struct authority {
    std::optional<std::string_view> userinfo;
    std::string_view host;
    std::optional<std::string_view> port;

    [[nodiscard]] bool valid () const noexcept;

    bool operator== (authority const& rhs) const;
    bool operator!= (authority const& rhs) const { return !operator== (rhs); }
  };

  std::optional<std::string_view> scheme;
  std::optional<struct authority> authority;
  struct path path;
  std::optional<std::string_view> query;
  std::optional<std::string_view> fragment;

  [[nodiscard]] bool valid () const noexcept;

  /// If an authority instance is present, return it otherwise an instance is
  /// created and returned.
  struct authority& ensure_authority () {
    return authority.has_value () ? *authority : authority.emplace ();
  }
  bool operator== (parts const& rhs) const;
  bool operator!= (parts const& rhs) const { return !operator== (rhs); }
};

std::ostream& operator<< (std::ostream& os, struct parts::path const& path);
std::ostream& operator<< (std::ostream& os,
                          struct parts::authority const& auth);
std::ostream& operator<< (std::ostream& os, parts const& p);

std::optional<parts> split (std::string_view in);

parts join (parts const& base, parts const& reference, bool strict = true);
std::optional<parts> join (std::string_view Base, std::string_view R,
                           bool strict = true);

std::string compose (parts const& p);
std::ostream& compose (std::ostream& os, parts const& p);

}  // end namespace uri

#endif  // URI_URI_HPP
