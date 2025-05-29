#ifndef __BOARD_H__
#define __BOARD_H__

#include <cassert>
#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "move.hpp"

namespace board {

class Board {
  // 120 squares board, contains type of piece on each square
  std::array<size_t, largeNC> board;

  // It consists of 3 boards, the first one
  // for white pawns, the second for black pawns, the third for both
  // colors, will be very helpful in the move generation phase
  std::array<size_t, 3> pawns;

  // It is useful to keep the positions of kings on the board for the
  // same reason, unsigned char is used because there are only 120 possible
  // fields for a move. In fact, we only need one  to store the
  // kings (120 for the first, 120 for the second, total 240, and 1
  // holds 255)
  std::array<unsigned char, 2> kings;

  // See Color enum
  Color side;

  // Place where en passant move is possible
  size_t enPas;

  // Counter from 0 to 50
  unsigned char fiftyMove;

  // Will be used with castle enum
  unsigned char castlePerm;

  // Amount half move in the current search
  // (1 move is the action of white and black, ply - only white)
  int ply;

  // Amount of all half moves has been made
  int hisPly;

  // Unique key of position
  size_t posKey;

  // Contains number of each piece type
  std::array<unsigned char, 13> pieceNum;

  // Big piece - every piece type that not a pawns
  // Same as `pawns` array
  std::array<unsigned char, 2> bigPiece;

  // Rooks and queens
  std::array<unsigned char, 2> majPiece;

  // Bishops and knights
  std::array<unsigned char, 2> minPiece;

  // Material score for black and white
  std::array<int, 2> material;

  // History of the game
  std::vector<move::Undo> history;

  // Piece list is a matrix that contain a type of piece in the current position
  // For example: we want to set white knight to the board on E1 square
  // so we just do - pList[wN][0] = E1;
  // This will help to look on all piece of current color
  // with that array we don't need to look through all empty squares
  std::array<std::array<unsigned char, 10>, 13> pieceList;

  // Strings for Board::dump
  static constexpr std::string_view pieceChar = ".PNBRQKpnbrqk";
  static constexpr std::string_view sideChar = "wb-";

  // Constructors
public:
  Board() { reset(); }
  Board(const std::string_view &fen) { parseFEN(fen); }

  // Getters
public:
  Color getSide() const { return side; }
  size_t getPiece(int square) const { return board[square]; }
  size_t getEnPas() const { return enPas; }
  unsigned char getPieceNum(int piece) const {
    return pieceNum[piece];
  }
  unsigned char getPieceListSq(int side, int num) const {
    return pieceList[side][num];
  }
  unsigned char getCastlePerm() const { return castlePerm; };

  // Public methods
public:
  // Reset most of the Board fields
  void reset();

  // Parsing FEN
  void parseFEN(const std::string_view &fen);

  // Generating unique position key
  size_t generate() const;

  // Dumping board and some statistics to screen
  void dump(std::ostream &os) const;

  // Updating piece lists
  void update();

  // Check all lists are correct
  void check() const;

  // Checking is square attacked by side
  bool isAttacked(const int sq, const Color side) const;
};

} // namespace board

#endif // __BOARD_H__
