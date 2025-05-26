#ifndef __MOVE_H__
#define __MOVE_H__

#include <array>
#include <string>

#include "board_constants.hpp"

// Forward declaration
// We cant #include "board.hpp" because of cyclic dependency
namespace board { class Board; }

namespace move {
 /*
  * We will coding one bit with all information of the move we need
  * Bits equaled to one are the bits that will define the word to the right
  
  * 0000 0000 0000 0000 0000 0111 1111
  * Position from; to get we use       -> & 0x7F

  * 0000 0000 0000 0011 1111 1000 0000
  * Position to;                       -> >> 7 & 0x7F

  * 0000 0000 0011 1100 0000 0000 0000
  * What piece was captured;           -> >> 14 & 0x3F

  * 0000 0000 0100 0000 0000 0000 0000
  * EnPas;                             -> & 0x40000

  * 0000 0000 1000 0000 0000 0000 0000
  * Pawn start;                        -> & 0x80000

  * 0000 1111 0000 0000 0000 0000 0000
  * In what piece pawn was Promoted;   -> >> 20 & 0xF

  * 0001 0000 0000 0000 0000 0000 0000
  * Castle;                            -> 0x1000000
  */

  constexpr int en_pas     = 0x40000;
  constexpr int pawn_start = 0x80000;
  constexpr int castle     = 0x1000000;

  class Move {
    int info;
    int score;
    std::string dump_str;

  public:
    Move() : info(0), score(0) {}
    Move(int i) : info(i), score(0) {}
    Move(int from, int to, int captured = 0, int promoted = 0, int fl = 0);

    std::string getDumpMove();

    int getInfo() const { return info; }
    int getScore() const { return score; }

    void SetInfo(int i) { info = i; }
    void SetScore(int s) { score = s; }

    int getFrom() const;
    int getTo() const;
    int getCaptured() const;
    int getPromoted() const;
    int getEnPas() const;
    int getPawnStart() const;
    int getCastle() const;
    int getPawnBits() const;
    int getPromotedBits() const;
  };

  class MoveList {
  private:
    std::array<Move, maxMoves> moves;
    size_t count;

  private:
    void AddQuietMove(const board::Board& b, Move&& m);
    void AddCapturedMove(const board::Board& b, Move&& m);
    void AddEnPasMove(const board::Board& b, Move&& m);

    void AddWhitePawnMove(const board::Board& b, Move&& m, bool captured);
    void AddBlackPawnMove(const board::Board& b, Move&& m, bool captured);
  public:
    MoveList() : count(0) {}

    Move& operator[] (int i) { return moves[i]; }
    void push_back(Move& m) { moves[count++] = m; }

    void dump(std::ostream& os);

    void GenerateAllMoves(const board::Board& b);
  };

  // Information to undo a move
  class Undo {
  public:
    int move;
    int castlePerm;
    int enPas;
    bool fiftyMove;
    size_t posKey;
  };

  char* getDumpSquare(const int sq);

} // namespace move

#endif // __MOVE_H__
