// Licensed after GNU GPL v3

#include <algorithm>
#include <fstream>
#include <numeric>
#include <filesystem>

#include <gtest/gtest.h>

#include <boost/preprocessor/repetition/repeat.hpp>

#include "../include/board.hpp"


namespace {
  size_t leaves;

  void perft(const unsigned char depth, board::Board& b) {
    b.check();

    if (depth == 0) {
      ++leaves;
      return;
    }

    move::MoveList l(b);

    std::for_each(l.begin(), l.end(), [&depth, &b, &l](auto elt) {
      if (b.makeMove(elt)) {
        perft(depth - 1, b);
        b.takeBackMove();
      }
    });
  }

  size_t perftTest(const unsigned char depth, board::Board& b) {
    b.check();

    leaves = 0;

    move::MoveList l(b);

    std::for_each(l.begin(), l.end(), [&depth, &b, &l](auto elt) {
      if (b.makeMove(elt)) {
        perft(depth - 1, b);
        b.takeBackMove();
      }
    });

    return leaves;
  }

  class perft_test : public ::testing::Test {
  protected:
    std::vector<std::pair<std::string, std::array<size_t, 6>>> tests;
    int N;
    static constexpr unsigned char depthMax = 6;

    void SetUp() override {
      std::ifstream file(PERFT_TESTS_PATH);

      for (std::string tmp; std::getline(file, tmp);) {
        std::string_view res(tmp);

        auto fend = res.find_first_of(";");

        std::string_view fen = res.substr(0, fend - 1);

        std::array<size_t, depthMax> depths;

        for (int i = 0; i < depthMax; ++i) {
          auto l = fend + 4;
          auto r = l + res.substr(l).find_first_of(" ");
          std::string_view depth = res.substr(l, r - l);
          depths[i] = std::stoull(std::string {depth});
          fend = r + 1;
        }
        tests.push_back({std::string{fen}, depths});
      }
      file.close();

#if defined(DEBUG_ONLY)
      N = 4;
#elif defined(RELEASE_ONLY)
      N = 6;
#endif
    }
  public:
    size_t size() { return tests.size(); }
  };
} // anonymous namespace

#define TEST_PERF(z, I, unused)              \
TEST_F(perft_test, board##I) {                 \
  auto&& [fen, depths] = tests[I];             \
  board::Board b{std::string_view(fen)};       \
  for (int i = 0; i < N; ++i)                  \
    ASSERT_EQ(perftTest(i + 1, b), depths[i]); \
}

BOOST_PP_REPEAT(PERFT_TESTS_NUM, TEST_PERF, ~)
