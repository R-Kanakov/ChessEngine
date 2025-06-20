// Licensed after GNU GPL v3

#include <gtest/gtest.h>

#include "../include/board_constants.hpp"

TEST(board_constants_test, board64) {
  constexpr std::array<unsigned char, 64> expected{
    21, 22, 23, 24, 25, 26, 27, 28,
    31, 32, 33, 34, 35, 36, 37, 38,
    41, 42, 43, 44, 45, 46, 47, 48,
    51, 52, 53, 54, 55, 56, 57, 58,
    61, 62, 63, 64, 65, 66, 67, 68,
    71, 72, 73, 74, 75, 76, 77, 78,
    81, 82, 83, 84, 85, 86, 87, 88,
    91, 92, 93, 94, 95, 96, 97, 98};

  ASSERT_EQ(expected, board64);
}

TEST(board_constants_test, board120) {
  constexpr std::array<unsigned char, 120> expected {
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   8,   9,  10,  11,  12,  13,  14,  15, 100,
    100,  16,  17,  18,  19,  20,  21,  22,  23, 100,
    100,  24,  25,  26,  27,  28,  29,  30,  31, 100,
    100,  32,  33,  34,  35,  36,  37,  38,  39, 100,
    100,  40,  41,  42,  43,  44,  45,  46,  47, 100,
    100,  48,  49,  50,  51,  52,  53,  54,  55, 100,
    100,  56,  57,  58,  59,  60,  61,  62,  63, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

  ASSERT_EQ(expected, board120);
}

TEST(board_constants_test, board) {
  using us = util::unique_sequence<unsigned char, EMPTY, regularNC>::type;
  using reset = util::sequence_in_shell<unsigned char, us, OFFBOARD>;
  constexpr auto arr = reset::get();

  constexpr std::array<unsigned char, 120> expected {
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

  ASSERT_EQ(expected, arr);
}

TEST(board_constants_test, ranks_board) {
  constexpr std::array<unsigned char, 120> expected {
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100,   0,   0,   0,   0,   0,   0,   0,   0, 100,
    100,   1,   1,   1,   1,   1,   1,   1,   1, 100,
    100,   2,   2,   2,   2,   2,   2,   2,   2, 100,
    100,   3,   3,   3,   3,   3,   3,   3,   3, 100,
    100,   4,   4,   4,   4,   4,   4,   4,   4, 100,
    100,   5,   5,   5,   5,   5,   5,   5,   5, 100,
    100,   6,   6,   6,   6,   6,   6,   6,   6, 100,
    100,   7,   7,   7,   7,   7,   7,   7,   7, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

  ASSERT_EQ(expected, bRanks);
}

TEST(board_constants_test, files_board) {
  constexpr std::array<unsigned char, 120> expected {
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100,   0,   1,   2,   3,   4,   5,   6,   7, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
    100, 100, 100, 100, 100, 100, 100, 100, 100, 100};

  ASSERT_EQ(expected, bFiles);
}

TEST(board_constants_test, set_mask_board) {
  std::array<size_t, regularNC> SetMask;
  for (int i= 0; i< regularNC; ++i)
    SetMask[i] = 0ull;
  for (int i = 0; i < regularNC; ++i)
    SetMask[i] |= (1ull << i);

  ASSERT_EQ(SetMask, setMask);
}

TEST(board_constants_test, clear_mask_board) {
  std::array<size_t, regularNC> ClearMask;
  for (int i = 0; i < regularNC; ++i)
    ClearMask[i] = 0ull;
  for (int i = 0; i < regularNC; ++i)
    ClearMask[i] |= ~(1ull << i);

  ASSERT_EQ(ClearMask, clearMask);
}