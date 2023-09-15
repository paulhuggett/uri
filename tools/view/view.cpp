//===- tools/view/view.cpp ------------------------------------------------===//
//*        _                *
//* __   _(_) _____      __ *
//* \ \ / / |/ _ \ \ /\ / / *
//*  \ V /| |  __/\ V  V /  *
//*   \_/ |_|\___| \_/\_/   *
//*                         *
//===----------------------------------------------------------------------===//
//
// Distributed under the MIT License.
// See https://github.com/paulhuggett/uri/blob/main/LICENSE.TXT
// for license information.
// SPDX-License-Identifier: MIT
//
//===----------------------------------------------------------------------===//
#include <iostream>
#include <vector>

#include "uri/pct_decode.hpp"

int main () {
  std::vector<char> n{'H', 'e', 'l', 'l', 'o', '%', '2',
                      '0', 'W', 'o', 'r', 'l', 'd'};
  auto lower = [] (auto c) {
    return static_cast<decltype (c)> (std::tolower (static_cast<int> (c)));
  };
  auto out = std::ostream_iterator<char>{std::cout, ""};
#if URI_PCTDECODE_RANGES
  auto v = n | uri::views::pctdecode | std::views::transform (lower);
  std::ranges::copy (v, out);
#else
  auto decoder = uri::pct_decoder{n};
  std::transform (decoder.begin (), decoder.end (), out, lower);
#endif
  std::cout << '\n';
}
