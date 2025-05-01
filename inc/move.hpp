#ifndef __MOVE_H__
#define __MOVE_H__

// We will coding one bit with all information of the move we need
// TODO: maybe make it with each variable separately
// Bits equaled to one are the bits that will define the word to the right

// 0000 0000 0000 0000 0000 0111 1111
// Position from, to get we use       -> 0x7F

// 0000 0000 0000 0011 1111 1000 0000
// Position to,                       -> >> 7 & 0x7F

// 0000 0000 0011 1100 0000 0000 0000
// What piece was captured,           -> >> 14 & 0x3F

// 0000 0000 0100 0000 0000 0000 0000
// EnPas,                             -> & 0x40000

// 0000 0000 1000 0000 0000 0000 0000
// Pawn start                         -> & 0x80000

// 0000 1111 0000 0000 0000 0000 0000
// In what piece pawn was Promoted,   -> >> 20 & 0xF

// 0001 0000 0000 0000 0000 0000 0000
// Castle,                            -> 0x1000000

namespace move {

  class Move {
    int info;
    int score;

  public:
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

  // Information to undo a move
  class Undo {
  public:
    int move;
    int castlePerm;
    int enPas;
    bool fiftyMove;
    size_t posKey;
  };

} // namespace move

#endif // __MOVE_H__
