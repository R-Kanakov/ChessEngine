// Licensed after GNU GPL v3

#ifndef __BOARD_HPP__
#define __BOARD_HPP__

#include <iomanip>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "move.hpp"

namespace board {
class Board {
  // 120 squares board, contains type of piece on each square
  std::array<unsigned char, largeNC> board;

  // It consists of 3 boards, each represented in 64 bits (ull):
  // 1. For white pawns
  // 2. For black pawns
  // 3. For pawns of both colors
  // Will be very helpful in the move generation phase
  std::array<size_t, 3> pawns;

  // It is useful to keep the positions of kings
  // on the board for the same reason.
  // `unsigned char` is used because there are only 120 possible
  // fields for a move.
  // In fact, we only need one to store the kings
  // (120 for the first, 120 for the second, total 240, while 1 holds 255)
  std::pair<unsigned char, unsigned char> kings;

  // See Color enum
  Color side;

  // Place where en passant move is possible
  Cell enPas;

  // Counter from 0 to 50
  unsigned char fiftyMove;

  // Will be used with castle enum
  unsigned char castlePerm;

  // Amount half move in the current search
  // (1 move is the action of white and black, ply - only white)
  size_t ply;

  // Amount of all half moves has been made
  // Indexing for the history vector (maybe unnessesary)
  size_t hisPly;

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
  std::array<unsigned, 2> material;

  // History of the game
  std::vector<move::Undo> history;

  // Piece list is a matrix that contain a type of piece in the current position
  // For example: we want to set white knight to the board on E1 square
  // so we just do => pList[wN][0] = E1;
  // This will help to look on all piece of current color
  // with that array we don't need to look through all empty squares
  std::array<std::array<unsigned char, 10>, 13> pieceList;

#if defined(DEBUG_ONLY)
  // Strings for Board::dump
  static constexpr std::string_view pieceChar = ".PNBRQKpnbrqk";
  static constexpr std::string_view sideChar = "wb-";
#endif

  // Constructors
public:
  Board() { reset(); }
  Board(const std::string_view &fen) { parseFEN(fen); }

  // Getters
public:
  Color getSide()   const { return side; }
  size_t getEnPas() const { return enPas; }
  
  unsigned char getPiece(unsigned char sq) const {
    return board[sq];
  }
  unsigned char getPieceNum(unsigned char piece) const {
    return pieceNum[piece];
  }
  unsigned char getPieceListSq(unsigned char side, unsigned char num) const {
    return pieceList[side][num];
  }
  unsigned char getCastlePerm() const {
    return castlePerm;
  };

  // Other methods
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
  bool isAttacked(const unsigned char sq, const Color side) const;

  // Methods that will change position key
private:
  void hashPiece(const unsigned char piece, const unsigned char sq)
                    { posKey ^= pieceKeys[piece][sq]; }
  void hashCastle() { posKey ^= castleKeys[castlePerm]; }
  void hashSide()   { posKey ^= sideKey; }
  void hashEnPas()  { posKey ^= pieceKeys[EMPTY][enPas]; }

  // Methods that will be used in makeMove
private:
  void clearPiece(const unsigned char sq);
  void addPiece(const unsigned char sq, const unsigned char piece);
  void movePiece(const unsigned char from, const unsigned char to);
  
public:
  void takeMove();
  bool makeMove(const move::Move& move);

};
} // namespace board

#endif // __BOARD_HPP__
