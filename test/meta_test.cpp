// Licensed after GNU GPL v3

#include <gtest/gtest.h>

#include "../include/util.hpp"

TEST(meta_test, integer_square_root) {
  constexpr auto N = util::Sqrt<unsigned, 987654>::value;
  unsigned expected = 993;

  ASSERT_EQ(N, expected);
}

TEST(meta_test, prime_number_1) {
  ASSERT_TRUE(util::is_prime<5>::value);
}

TEST(meta_test, prime_number_2) {
  ASSERT_FALSE(util::is_prime<6>::value);
}

template <int... T>
struct has_type {
  using type = int;
};

TEST(meta_test, existence_of_type_1) {
  ASSERT_TRUE(util::has_typedef_type<has_type<>>::value);
}

template<int... T> struct no_type {};

TEST(meta_test, existence_of_type_2) {
  ASSERT_FALSE(util::has_typedef_type<no_type<>>::value);
}

TEST(meta_test, existence_of_type_3) {
  using cs = util::ñonsecutive_sequence<int, 1, 10>;
  ASSERT_TRUE(util::has_typedef_type<cs>::value);
}

TEST(meta_test, two_closest_production_1) {
  using cs = util::ñonsecutive_sequence<int, 1, 10>::type;
  constexpr size_t size = cs {}.size();
  constexpr auto p = util::find_pair<size>();

  ASSERT_EQ(p.first, 5);
  ASSERT_EQ(p.second, 2);
}

TEST(meta_test, two_closest_production_2) {
  using cs = util::ñonsecutive_sequence<int, 1, 15>::type;
  constexpr size_t size = cs {}.size();
  constexpr auto p = util::find_pair<size>();

  ASSERT_EQ(p.first, 5);
  ASSERT_EQ(p.second, 3);
}

TEST(meta_test, two_closest_production_3) {
  using cs = util::ñonsecutive_sequence<int, 0, 63>::type;
  constexpr size_t size = cs {}.size();
  constexpr auto p = util::find_pair<size>();

  ASSERT_EQ(p.first, 8);
  ASSERT_EQ(p.second, 8);
}