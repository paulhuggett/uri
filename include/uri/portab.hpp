#ifndef URI_PORTAB_HPP
#define URI_PORTAB_HPP

#include <cstdint>
#include <functional>

#if __has_include(<version>)
#include <version>
#endif

namespace uri {

#if defined(__cpp_lib_ranges_starts_ends_with) && \
  __cpp_lib_ranges_starts_ends_with >= 202106L
inline constexpr auto starts_with = std::ranges::starts_with;
#else
struct starts_with_fn {
  template <std::input_iterator I1, std::sentinel_for<I1> S1,
            std::input_iterator I2, std::sentinel_for<I2> S2,
            typename Pred = std::ranges::equal_to,
            typename Proj1 = std::identity, typename Proj2 = std::identity>
    requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
  constexpr bool operator() (I1 first1, S1 last1, I2 first2, S2 last2,
                             Pred pred = {}, Proj1 proj1 = {},
                             Proj2 proj2 = {}) const {
    return std::ranges::mismatch (std::move (first1), last1, std::move (first2),
                                  last2, std::move (pred), std::move (proj1),
                                  std::move (proj2))
             .in2 == last2;
  }

  template <std::ranges::input_range R1, std::ranges::input_range R2,
            typename Pred = std::ranges::equal_to,
            typename Proj1 = std::identity, typename Proj2 = std::identity>
    requires std::indirectly_comparable<std::ranges::iterator_t<R1>,
                                        std::ranges::iterator_t<R2>, Pred,
                                        Proj1, Proj2>
  constexpr bool operator() (R1&& r1, R2&& r2, Pred pred = {}, Proj1 proj1 = {},
                             Proj2 proj2 = {}) const {
    return (*this) (std::ranges::begin (r1), std::ranges::end (r1),
                    std::ranges::begin (r2), std::ranges::end (r2),
                    std::move (pred), std::move (proj1), std::move (proj2));
  }
};

inline constexpr starts_with_fn starts_with{};
#endif  // __cpp_lib_ranges_starts_ends_with

#if defined(__cpp_lib_ranges_find_last) && __cpp_lib_ranges_find_last >= 202207L
inline constexpr auto find_last = std::ranges::find_last;
#else
struct find_last_fn {
  template <std::forward_iterator I, std::sentinel_for<I> S, class T,
            class Proj = std::identity>
    requires std::indirect_binary_predicate<std::ranges::equal_to,
                                            std::projected<I, Proj>, T const*>
  constexpr std::ranges::subrange<I> operator() (I first, S last,
                                                 const T& value,
                                                 Proj proj = {}) const {
    // Note: if I is mere forward_iterator, we may only go from begin to end.
    I found{};
    for (; first != last; ++first) {
      if (std::invoke (proj, *first) == value) {
        found = first;
      }
    }

    if (found == I{}) {
      return {first, first};
    }
    return {found, std::ranges::next (found, last)};
  }

  template <std::ranges::forward_range R, class T, class Proj = std::identity>
    requires std::indirect_binary_predicate<
      std::ranges::equal_to, std::projected<std::ranges::iterator_t<R>, Proj>,
      const T*>
  constexpr std::ranges::borrowed_subrange_t<R> operator() (
    R&& r, const T& value, Proj proj = {}) const {
    return this->operator() (std::ranges::begin (r), std::ranges::end (r),
                             value, std::ref (proj));
  }
};

inline constexpr find_last_fn find_last;
#endif  // __cpp_lib_ranges_find_last

}  // end namespace uri

#endif  // URI_PORTAB_HPP
