//===- include/uri/uri.hpp --------------------------------*- mode: C++ -*-===//
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
#ifndef URI_URI_HPP
#define URI_URI_HPP

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace uri {

struct path {
  bool absolute = false;
  std::vector<std::string> segments;

  constexpr bool operator== (path const& rhs) const {
    return absolute == rhs.absolute && segments == rhs.segments;
  }
  constexpr bool operator!= (path const& rhs) const {
    return !operator== (rhs);
  }

  // Remove dot segments from the path.
  void remove_dot_segments ();
  [[nodiscard]] constexpr bool empty () const noexcept {
    return !absolute && segments.empty ();
  }
  explicit operator std::string () const;
  explicit operator std::filesystem::path () const;
};

struct authority {
  std::optional<std::string> userinfo;
  std::optional<std::string> host;
  std::optional<std::string> port;

  constexpr explicit operator bool () const { return userinfo || host || port; }
  constexpr bool operator== (authority const& rhs) const {
    return userinfo == rhs.userinfo && host == rhs.host && port == rhs.port;
  }
  constexpr bool operator!= (authority const& rhs) const {
    return !operator== (rhs);
  }
};

std::ostream& operator<< (std::ostream& os, authority const& auth);

struct parts {
  std::optional<std::string> scheme;
  struct authority authority;
  struct path path;
  std::optional<std::string> query;
  std::optional<std::string> fragment;

  constexpr bool operator== (parts const& rhs) const {
    return scheme == rhs.scheme && authority == rhs.authority &&
           path == rhs.path && query == rhs.query && fragment == rhs.fragment;
  }
  constexpr bool operator!= (parts const& rhs) const {
    return !operator== (rhs);
  }
};

std::optional<parts> split (std::string_view in);

parts join (parts const& base, parts const& reference, bool strict = true);
std::optional<parts> join (std::string_view Base, std::string_view R,
                           bool strict = true);

std::string compose (parts const& p);
std::ostream& compose (std::ostream& os, parts const& p);
std::ostream& operator<< (std::ostream& os, parts const& p);

}  // end namespace uri

#endif  // URI_URI_HPP
