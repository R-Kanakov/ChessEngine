// Licensed after GNU GPL v3

#include "init.hpp"

// TODO: move all checks to tests

void printBoard120(auto b) {
  std::cout << "\n\nBoard120\n";
  for (int i = 0; i < largeNC; ++i) {
    if (i % 10 == 0)
      std::cout << "\n";
    std::cout << std::setw(4) << static_cast<int>(b[i]) << ",";
  }
}

void printBoard64(auto b) {
  std::cout << "\n\nBoard64\n";
  for (int i = 0; i < regularNC; ++i) {
    if (i % 8 == 0)
      std::cout << "\n";
    std::cout << std::setw(4) << static_cast<int>(b[i]);
  }
}


// Two queens 8/8/8/2Q5/8/6q1/8/8 w - - 0 1
// Two rooks  8/8/8/2r5/8/6R1/8/8 w - - 0 1

// e4  rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1
// d5  rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2
// Nf3 rnbqkbnr/ppp1pppp/8/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2
// Kd7 rnbq1bnr/pppkpppp/8/3p4/4P3/5N2/PPPP1PPP/RNBQKB1R w KQ - 2 3

// pos r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1

// pawnsW rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1
// pawnsB rnbqkb1r/p1p1p3/3p3p/1p1p4/2P1Pp2/8/PP1P1PpP/RNBQKB1R b - e3 0 1

// knights 5k2/1n6/4n3/6N1/8/3N4/8/5K2 b - - 0 1
// rooks   6k1/8/5r2/8/1nR5/5N2/8/6K1 w - - 0 1
// queens  6k1/8/4nq2/8/1nQ5/5N2/1N6/6K1 w - - 0 1
// bishops 6k1/1b6/4n3/8/1n4B1/1B3N2/1N6/2b3K1 b - - 0 1

// castle1 r3k2r/8/8/8/8/8/8/R3K2R b KQkq - 0 1
// castle2 3rk2r/8/8/8/8/8/6p1/R3K2R w KQkq - 0 1
// castle3 r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1


int main() {
  allInit();

  board::Board b("rnbqkbnr/ppp1pppp/8/3p4/4P3/8/PPPP1PPP/RNBQKBNR w KQKq d6 0 2");
  b.dump(std::cout);

  size_t board = 0;
  board |= 1ull << convert120To64(G2);
  printBitBoard(board);


  //b.reset();
  b.check();

  move::Move m;
  int from = A2, to = H7;
  int cap = wR, prom = bK;

  m.SetInfo(from | to << 7 | cap << 14 | prom << 20);

  std::cout << "\nFrom: " << m.getFrom() << ", to: " << m.getTo() << ", cap: " 
            << m.getCaptured() << ", prom: " << m.getPromoted() << "\n";

  std::cout << "Algebraic from: " << move::getDumpSquare(from) << "\n";
  std::cout << "Algebraic to: " << move::getDumpSquare(to) << "\n";
  std::cout << "Algebraic move: " << m.getDumpMove() << "\n";

  b.parseFEN("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
  b.dump(std::cout);
  b.check();

  move::MoveList l;

  l.GenerateAllMoves(b);

  l.dump(std::cout);

  return 0;
}

