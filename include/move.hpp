// Licensed after GNU GPL v3

#ifndef __MOVE_HPP__
#define __MOVE_HPP__

#include <array>
#include <string>

#include "board_constants.hpp"

// Forward declaration
// We cant #include "board.hpp" because of cyclic dependency
namespace board { class Board; }

namespace move {
/*
 * We will coding one bit with all information of the move we need
 * Bits equaled to one are the bits that will define the word bellow

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
 * Castle;                            -> & 0x1000000
 */

class Move {
  int info;
  int score;
  std::string dump_str;

public:
  Move() : info(0), score(0) {}
  Move(int i) : info(i), score(0) {}
  Move(unsigned char from, unsigned char to, unsigned char captured = 0,
       unsigned char promoted = 0, unsigned int flag = 0)
      : info(from | (to << 7) | (captured << 14) | (promoted << 20) | flag),
        score(0) {}

  std::string getDumpMove();

  int getInfo()  const { return info; }
  int getScore() const { return score; }

  void SetInfo(int i)  { info  = i; }
  void SetScore(int s) { score = s; }

  unsigned char getFrom()         const { return info & 0x7f; }
  unsigned char getTo()           const { return (info >> 7) & 0x7f; }
  unsigned char getCaptured()     const { return (info >> 14) & 0xf; }
  unsigned char getPromoted()     const { return (info >> 20) & 0xf; }
  bool getEnPas()                 const { return info & 0x40000; }
  bool getPawnStart()             const { return info & 0x80000; }
  bool getCastle()                const { return info & 0x100000; }
  unsigned char getPawnBits()     const { return info & 0x7c000; }
  unsigned char getPromotedBits() const { return info & 0xf00000; }
};

class MoveList {
private:
  std::array<Move, maxMoves> moves;
  size_t count;

private:
  void AddQuietMove(const board::Board &b, Move &&m);
  void AddCapturedMove(const board::Board &b, Move &&m);
  void AddEnPasMove(const board::Board &b, Move &&m);

  void AddPawnMove(const board::Board &b, Move &&m, Color side, bool captured);
public:
  MoveList() : count(0) {}

  const Move &operator[](int i) const { return moves[i]; }
  void push_back(Move &m) { moves[count++] = m; }

  void dump(std::ostream &os) const;

  void GenerateAllMoves(const board::Board &b);
};

// Information to undo a move
class Undo {
  Move move;
  unsigned char castlePerm;
  Cell enPas;
  unsigned char fiftyMove;
  size_t posKey;

  // Constructor
public:
  Undo(Move m, int cp, Cell ep, unsigned char fm, size_t pk)
      : move(m), castlePerm(cp), enPas(ep), fiftyMove(fm), posKey(pk) {}

  // Getters
public:
  Move getMove()                const { return move; }
  unsigned char getCastlePerm() const { return castlePerm; }
  Cell getEnPas()               const { return enPas; }
  unsigned char getFiftyMove()  const { return fiftyMove; }

};

char *getDumpSquare(const int sq);
} // namespace move

#endif // __MOVE_HPP__

// Makemove plan:

// function make(Move move, Board* pos)
// 1. Get `from`, `to`, `cap` from the move
// 2. Store the current position in the pos->history vector
// 3. Move the current piece from `from` to `to`
// 3.1. If a capture was made, remove captured from the piece list
// 4. Update fifty move rule, see if pawn was moved
// 5. Promote pawn to something if necessary
// 5. EnPas captures check
// 6. Set EnPas flag if pawn went in 2 squares
// 7. For all pieces added, moved, removed, update all position counters and piece lists
// 8. Maintain a hashkey
// 9. Change side, increment ply and hisPly