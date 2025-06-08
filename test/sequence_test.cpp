// Licensed after GNU GPL v3

#include <gtest/gtest.h>

#include "../include/util.hpp"

TEST(sequence_test, sequence_size) {
  using cs = util::сonsecutive_sequence<int, 1, 10>::type;
  constexpr size_t N = cs{}.size();
  ASSERT_EQ(N, 10);
}

TEST(sequence_test, sequence_with_two_gaps_1) {
  using swg = util::sequence_with_two_gaps<int, 1, 50>::type;
  constexpr auto arr = util::stoa(swg{});

  constexpr std::array<int, swg::size()> expected {
      1,  2,  3,  4,  5,  6,  7,  8,
      11, 12, 13, 14, 15, 16, 17, 18,
      21, 22, 23, 24, 25, 26, 27, 28,
      31, 32, 33, 34, 35, 36, 37, 38,
      41, 42, 43, 44, 45, 46, 47, 48};

  ASSERT_EQ(arr, expected);
}

TEST(sequence_test, sequence_with_two_gaps_2) {
  using swg_1 = util::sequence_with_two_gaps<int, 1, 48>::type;
  constexpr auto arr_1 = stoa(swg_1{});

  using swg_2 = util::sequence_with_two_gaps<int, 1, 49>::type;
  constexpr auto arr_2 = util::stoa(swg_2{});

  ASSERT_EQ(arr_1, arr_2);
}

TEST(sequence_test, consecutive_sequence) {
  using cs = util::сonsecutive_sequence<int, 1, 10>::type;
  constexpr auto arr = util::stoa(cs{});
  constexpr std::array<int, 10> expected {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

  ASSERT_EQ(arr, expected);
}

TEST(sequence_test, sequence_in_shell) {
  using cs = util::сonsecutive_sequence<int, 1, 4>::type;
  using cs_in_shell = util::sequence_in_shell<int, cs, 5>;
  constexpr auto arr = cs_in_shell{}.get();

  constexpr std::array<int, 24> expected{
    5, 5, 5, 5,
    5, 5, 5, 5,
    5, 1, 2, 5,
    5, 3, 4, 5,
    5, 5, 5, 5,
    5, 5, 5, 5};

  ASSERT_EQ(arr, expected);
}

TEST(sequence_test, sequence_transform_1) {
  using sts = util::sequence_transform<int, util::shift<int>, 1, 10>::type;
  constexpr auto arr = util::stoa(sts{});
  constexpr std::array<int, 10> expected{2, 4, 8, 16, 32, 64, 128, 256, 512, 1024};

  ASSERT_EQ(arr, expected);
}

TEST(sequence_test, sequence_transform_2) {
  using sts = util::sequence_transform<unsigned, util::shift_with_tilda<unsigned>, 1, 10> ::type;
  constexpr auto arr = util::stoa(sts {});
  constexpr std::array<unsigned, 10> expected{
      4294967293, 4294967291, 4294967287, 4294967279, 4294967263,
      4294967231, 4294967167, 4294967039, 4294966783, 4294966271};

  ASSERT_EQ(arr, expected);
}

TEST(sequence_test, sequence_concat_1) {
  using cs = util::сonsecutive_sequence<int, 1, 4>::type;
  using concat_5_cs = util::n_sequences<cs, 5>::type;
  constexpr auto arr = util::stoa(concat_5_cs{});
  constexpr std::array<int, 20> expected{1, 2, 3, 4,
                                         1, 2, 3, 4,
                                         1, 2, 3, 4,
                                         1, 2, 3, 4,
                                         1, 2, 3, 4};
  ASSERT_EQ(arr, expected);
}

TEST(sequence_test, sequence_unique) {
  using us = util::unique_sequence<int, 1, 4>::type;
  constexpr auto arr = util::stoa(us{});
  constexpr std::array<int, 4> expected {1, 1, 1, 1};
  ASSERT_EQ(arr, expected);
}

TEST(sequence_test, sequence_columnar) {
  using us = util::columnar_sequence<int, 1, 4, 4>::type;
  constexpr auto arr = util::stoa(us {});
  constexpr std::array<int, 16> expected {1, 1, 1, 1,
                                          2, 2, 2, 2,
                                          3, 3, 3, 3,
                                          4, 4, 4, 4};
  ASSERT_EQ(arr, expected);
}
