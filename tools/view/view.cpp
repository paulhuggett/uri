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
#include <algorithm>
#include <cassert>
#include <concepts>
#include <cstddef>
#include <iostream>
#include <optional>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>

inline constexpr std::byte bad = std::byte{0b1'0000};

/// Convert the argument character from a hexadecimal character code
/// (A-F/a-f/0-9) to an integer in the range 0-15. If the input character is
/// not a valid hex character code, returns pct_decode::bad.
template <std::integral ValueT>
constexpr std::byte hex2dec (ValueT const digit) noexcept {
  if (digit >= 'a' && digit <= 'f') {
    return static_cast<std::byte> (static_cast<unsigned> (digit) - ('a' - 10));
  }
  if (digit >= 'A' && digit <= 'F') {
    return static_cast<std::byte> (static_cast<unsigned> (digit) - ('A' - 10));
  }
  if (digit >= '0' && digit <= '9') {
    return static_cast<std::byte> (static_cast<unsigned> (digit) - '0');
  }
  return bad;
}
static bool either_bad (std::byte n1, std::byte n2) noexcept {
  return ((n1 | n2) & bad) != std::byte{0};
}

template <std::ranges::input_range View>
  requires std::ranges::forward_range<View>
class pctdecode_view
    : public std::ranges::view_interface<pctdecode_view<View>> {
  class iterator;
  class sentinel;

public:
  pctdecode_view ()
    requires std::default_initializable<View>
  = default;

  constexpr explicit pctdecode_view (View base) : base_{std::move (base)} {}

  template <typename Vp = View>
  constexpr View base () const&
    requires std::copy_constructible<Vp>
  {
    return base_;
  }
  constexpr View base () && { return std::move (base_); }

  constexpr iterator begin () { return {*this, std::ranges::begin (base_)}; }

  constexpr auto end () {
    if constexpr (std::ranges::common_range<View>) {
      return iterator{*this, std::ranges::end (base_)};
    } else {
      return sentinel{*this};
    }
  }

private:
  [[no_unique_address]] View base_ = View{};
};

template <class Range>
pctdecode_view (Range&&) -> pctdecode_view<std::views::all_t<Range>>;

template <std::ranges::input_range View>
  requires std::ranges::forward_range<View>
class pctdecode_view<View>::iterator {
public:
  using iterator_concept = std::forward_iterator_tag;
  using iterator_category = std::forward_iterator_tag;
  using value_type = std::ranges::range_value_t<View>;
  using difference_type = std::ranges::range_difference_t<View>;

  iterator ()
    requires std::default_initializable<std::ranges::iterator_t<View>>
  = default;

  constexpr iterator (pctdecode_view& parent,
                      std::ranges::iterator_t<View> current)
      : parent_{std::addressof (parent)}, current_{std::move (current)} {}

  constexpr std::ranges::iterator_t<View> const& base () const& noexcept {
    return current_;
  }
  constexpr std::ranges::iterator_t<View> base () && {
    return std::move (current_);
  }

  constexpr std::ranges::range_reference_t<View> operator* () const {
    auto& c = *current_;
    if (c != '%' ||
        std::distance (current_, std::ranges::end (parent_->base_)) < 3) {
      return c;
    }
    auto const nhi = hex2dec (*(current_ + 1));
    auto const nlo = hex2dec (*(current_ + 2));
    // If either character isn't valid hex, then return the original.
    if (either_bad (nhi, nlo)) {
      return c;
    }
    hex_ = static_cast<value_type> ((nhi << 4) | nlo);
    return hex_;
  }
  constexpr std::ranges::iterator_t<View> operator->() const
    requires std::copyable<std::ranges::iterator_t<View>>
  {
    return &(**this);
  }

  constexpr iterator& operator++ () {
    // Remove 1 character unless we've got a '%' followed by two legal hex
    // characters in which case we remove 3.
    std::advance (
      current_,
      std::distance (current_, std::ranges::end (parent_->base_)) >= 3 &&
          *current_ == '%' &&
          !either_bad (hex2dec (*(current_ + 1)), hex2dec (*(current_ + 2)))
        ? 3
        : 1);
    return *this;
  }

  constexpr iterator operator++ (int) {
    auto old = *this;
    ++(*this);
    return old;
  }

  friend constexpr bool operator== (iterator const& x, iterator const& y)
    requires std::equality_comparable<std::ranges::iterator_t<View>>
  {
    return x.current_ == y.current_;
  }

  friend constexpr std::ranges::range_rvalue_reference_t<View>
  iter_move (iterator const& it) noexcept (
    noexcept (std::ranges::iter_move (it.current_))) {
    return std::ranges::iter_move (it.current_);
  }

  friend constexpr void iter_swap (
    iterator const& x,
    iterator const& y) noexcept (noexcept (std::ranges::iter_swap (x.current_,
                                                                   y.current_)))
    requires std::indirectly_swappable<std::ranges::iterator_t<View>>
  {
    return std::ranges::iter_swap (x.current_, y.current_);
  }

private:
  [[no_unique_address]] pctdecode_view* parent_ = nullptr;
  [[no_unique_address]] std::ranges::iterator_t<View> current_ =
    std::ranges::iterator_t<View> ();
  mutable std::ranges::range_value_t<View> hex_ = 0;
};

template <std::ranges::input_range View>
  requires std::ranges::forward_range<View>
class pctdecode_view<View>::sentinel {
public:
  sentinel () = default;
  constexpr explicit sentinel (pctdecode_view& parent)
      : end_{std::ranges::end (parent.base_)} {}

  constexpr std::ranges::sentinel_t<View> base () const { return end_; }
  friend constexpr bool operator== (iterator const& x, sentinel const& y) {
    return x.current_ == y.end_;
  }

private:
  std::ranges::sentinel_t<View> end_{};
};

namespace details {

struct pctdecode_range_adaptor {
  template <std::ranges::viewable_range Range>
  constexpr auto operator() (Range&& r) const {
    return pctdecode_view{std::forward<Range> (r)};
  }
};

template <std::ranges::viewable_range Range>
constexpr auto operator| (Range&& r, pctdecode_range_adaptor const& a) {
  return a (std::forward<Range> (r));
}

}  // end namespace details

namespace views {
inline constexpr auto pctdecode_filter = details::pctdecode_range_adaptor{};
}

int main () {
  std::vector<char> n{'H', 'e', 'l', 'l', 'o', '%', '2',
                      '0', 'W', 'o', 'r', 'l', 'd'};
  auto v = n | views::pctdecode_filter;
  std::ranges::copy (v, std::ostream_iterator<char> (std::cout, ""));
  std::cout << '\n';
}
