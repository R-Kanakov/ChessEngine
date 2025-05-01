#include "init.hpp"

void printBoard120() {
  std::cout << "\n\nBoard120\n";
  for (int i = 0; i < largeNC; ++i) {
    if (i % 10 == 0)
      std::cout << "\n";
    std::cout << std::setw(4) << Board120[i];
  }
}

void printBoard64() {
  std::cout << "\n\nBoard64\n";
  for (int i = 0; i < regularNC; ++i) {
    if (i % 8 == 0)
      std::cout << "\n";
    std::cout << std::setw(4) << Board64[i];
  }
}

// Two queens 8/8/8/2Q5/8/6q1/8/8 w - - 0 1
// Two rooks  8/8/8/2r5/8/6R1/8/8 w - - 0 1
// e4  rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
// d5  rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2
// Nf3 rnbqkbnr/ppp1pppp/8/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2
// Kd7 rnbq1bnr/pppkpppp/8/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R w KQ - 2 3
// pos r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1

int main() {

  allInit();

  board::Board b;
  b.parseFEN("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");
  b.dump(std::cout);

  size_t board = 0;
  board |= 1ull << convert120To64(G2);
  printBitBoard(board);

  printBoard64();
  printBoard120();

  //b.reset();
  b.check();

  return 0;
}

// Next steps:
// Do/Undo move
//
// Unit testing
//
// Evaluation
// Search
//
// Connect GUI (uci)
