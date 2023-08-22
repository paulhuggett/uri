
#include <gmock/gmock.h>

#include <string>
#include <vector>

#include "rule.hpp"

using testing::ElementsAre;
using namespace std::string_literals;

struct Rule : public testing::Test {
  std::vector<std::string> output;

  auto remember () {
    return [this] (std::string_view str) { output.emplace_back (str); };
  }
};
// NOLINTNEXTLINE
TEST_F (Rule, Concat) {
  bool ok =
    rule ("ab")
      .concat ([] (rule const& r) { return r.single_char ('a'); }, remember ())
      .concat ([] (rule const& r) { return r.single_char ('b'); }, remember ())
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "b"));
}
// NOLINTNEXTLINE
TEST_F (Rule, ConcatAcceptorOrder) {
  bool ok =
    rule ("ab")
      .concat (
        [this] (rule const& r) {
          return r
            .concat ([] (rule const& r1) { return r1.single_char ('a'); },
                     remember ())
            .concat ([] (rule const& r2) { return r2.single_char ('b'); },
                     remember ())
            .matched ("ab", r);
        },
        [this] (std::string_view str) {
          output.push_back ("post "s + std::string{str});
        })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "b", "post ab"));
}
// NOLINTNEXTLINE
TEST_F (Rule, FirstAlternative) {
  bool ok =
    rule ("ab")
      .concat (single_char ('a'), remember ())
      .alternative (
        [this] (rule const& r) {
          return r.concat (single_char ('b'), remember ()).matched ("b", r);
        },
        [this] (rule const& r) {
          return r.concat (single_char ('c'), remember ()).matched ("c", r);
        })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "b"));
}
// NOLINTNEXTLINE
TEST_F (Rule, SecondAlternative) {
  bool ok =
    rule ("ac")
      .concat (single_char ('a'), remember ())
      .alternative (
        [this] (rule const& r) {
          return r.concat (single_char ('b'), remember ()).matched ("b", r);
        },
        [this] (rule const& r) {
          return r.concat (single_char ('c'), remember ()).matched ("c", r);
        })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "c"));
}
// NOLINTNEXTLINE
TEST_F (Rule, AlternativeFail) {
  bool ok =
    rule ("ad")
      .concat (single_char ('a'), remember ())
      .alternative (
        [this] (rule const& r) {
          return r.concat (single_char ('b'), remember ()).matched ("b", r);
        },
        [this] (rule const& r) {
          return r.concat (single_char ('c'), remember ()).matched ("c", r);
        })
      .done ();
  EXPECT_FALSE (ok);
  EXPECT_TRUE (output.empty ());
}
// NOLINTNEXTLINE
TEST_F (Rule, Star) {
  bool ok =
    rule ("aaa")
      .star ([this] (rule const& r) {
        return r.concat (single_char ('a'), remember ()).matched ("a", r);
      })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "a", "a"));
}
// NOLINTNEXTLINE
TEST_F (Rule, StarConcat) {
  bool ok =
    rule ("aaab")
      .star ([this] (rule const& r) {
        return r.concat (single_char ('a'), remember ()).matched ("a", r);
      })
      .concat (single_char ('b'), remember ())
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "a", "a", "b"));
}
// NOLINTNEXTLINE
TEST_F (Rule, Star2) {
  bool ok =
    rule ("/")
      .star ([this] (rule const& r1) {
        return r1.concat (single_char ('/'), remember ())
          .concat (
            [] (rule const& r2) {
              return r2.star (char_range ('a', 'z')).matched ("a-z", r2);
            },
            remember ())
          .matched ("*(a-z)", r1);
      })
      .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("/", ""));
}
// NOLINTNEXTLINE
TEST_F (Rule, OptionalPresent) {
  bool ok = rule ("abc")
              .concat (single_char ('a'), remember ())
              .optional (single_char ('b'), remember ())
              .concat (single_char ('c'), remember ())
              .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "b", "c"));
}
// NOLINTNEXTLINE
TEST_F (Rule, OptionalNotPresent) {
  bool ok = rule ("ac")
              .concat (single_char ('a'), remember ())
              .optional (single_char ('b'), remember ())
              .concat (single_char ('c'), remember ())
              .done ();
  EXPECT_TRUE (ok);
  EXPECT_THAT (output, ElementsAre ("a", "c"));
}
