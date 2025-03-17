#ifndef __BOARD_H__
#define __BOARD_H__

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <unordered_map> // TODO: maybe change it to absl or boost
#include <vector>

#include "util.h"

// Color
enum { WHITE, BLACK, BOTH };

// NC - Number of Cells
constexpr int largeNC = 120;
constexpr int regularNC = 64;

constexpr std::array<unsigned char, 64> bitTable{
    63, 30, 3,  32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34,
    61, 29, 2,  51, 21, 43, 45, 10, 18, 47, 1,  54, 9,  57, 0,  35,
    62, 31, 40, 4,  49, 5,  52, 26, 60, 6,  23, 44, 46, 27, 56, 16,
    7,  39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

constexpr std::string_view startPos =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Two arrays that can give back file and rank from square
// TODO: Use `util.h` to init
constexpr std::array<unsigned char, largeNC> bFiles{};
constexpr std::array<unsigned char, largeNC> bRanks{};

// Strings for Board::dump, probably they should be static constexpr
// and in Board::
constexpr std::string_view pieceChar = ".PNBRQKpnbrqk";
constexpr std::string_view sideChar = "wb-";
constexpr std::string_view rankChar = "12345678";
constexpr std::string_view fileChar = "abcdefgh";

// Is piece big, major, min, what value is it, what color is it
constexpr std::array pieceBig{false, false, true, true, true, true, true,
                              false, true,  true, true, true, true};
constexpr std::array pieceMaj{false, false, false, false, true, true, true,
                              false, false, false, true,  true, true};
constexpr std::array pieceMin{false, false, true, true,  false, false, false,
                              false, true,  true, false, false, false};
constexpr std::array<int, 13> pieceVal{0,   100, 325, 550, 1000, 50000,
                                       100, 325, 325, 550, 1000, 50000};
constexpr std::array<unsigned char, 13> pieceCol{
    BOTH,  WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
    BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

extern std::array<int, largeNC> Board120;

using range64 = typename util::make_range_sequence_with_two_gaps<21, 98>::type;
constexpr std::array<int, 64> Board64 =
    util::make_array_from_sequence(range64{});

enum { EMPTY, wP, wN, wB, wR, wQ, wK, bP, bN, bB, bR, bQ, bK };

enum { FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H };
enum { RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8 };

// Cells enum
enum {
  A1 = 21, B1, C1, D1, E1, F1, G1, H1,
  A2 = 31, B2, C2, D2, E2, F2, G2, H2,
  A3 = 41, B3, C3, D3, E3, F3, G3, H3,
  A4 = 51, B4, C4, D4, E4, F4, G4, H4,
  A5 = 61, B5, C5, D5, E5, F5, G5, H5,
  A6 = 71, B6, C6, D6, E6, F6, G6, H6,
  A7 = 81, B7, C7, D7, E7, F7, G7, H7,
  A8 = 91, B8, C8, D8, E8, F8, G8, H8,
  NO_SQ,
  OFFBOARD
};

// Each bit determines the possibility if castling
// 1011 - black queen can castle only to king's side, white can do both
enum { WKC = 1, WQC = 2, BKC = 4, BQC = 8 };

// Information to undo a move
class Undo {
public:
  int move;
  int castlePerm;
  int enPas;
  bool fiftyMove;
  size_t posKey;
};

class Board {
  // 120 squares board, contains type of piece on each square
  std::array<unsigned char, largeNC> board;

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
  int side;

  // Place where en passant is possible
  int enPas;

  // Counter from 0 to 50
  unsigned char fiftyMove;

  // Will be used with castle enum
  int castlePerm;

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

  // History of the game, equals to 24 kB...
  // Maybe we should contain history only for the maximum depth size
  // We can search though this array and find same position with same posKey
  // Should be std::vector and increase with history increase
  std::vector<Undo> history;

  // Piece list is a matrix that contain a type of piece in the current position
  // For example: we want to set white knight to the board on E1 square
  // so we just do - pList[wN][0] = E1;
  // This will help to look on all piece of current color
  // with that array we don't need to look through all empty squares
  std::array<std::array<unsigned char, 10>, 13> pieceList;

public:
  // Reset most of the Board fields
  void reset();

  // Parsing FEN
  int parseFEN(const std::string_view &fen);

  // Generating unique position key
  size_t generate() const;

  // Dumping board and some statistics to screen
  void dump(std::ostream &os) const;

  // Updating piece lists
  void update();

  // Check all lists are correct
  void check() const;

  // Checking is square attacked by side
  bool isAttacked(const int sq, const bool side) const;
};

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

class Move {
  int info;
  int score;

public:
  int getFrom();
  int getTo();
  int getCaptured();
  int getPromoted();
  int getEnPas();
  int getPawnStart();
  int getCastle();
  int getPawnBits();
  int getPromotedBits();
};

// Basic conversion from [file, rank] to index of 120 squares board
int convertFR(int file, int rank);

// Convert index from 120 squares board to the index of 64 squares board
inline int convert120To64(int sq);

// Convert index from 64 squares board to the index of 120 squares board
inline int convert64To120(int sq);

// Printing the board to the console
void printBitBoard(size_t bb);

// Returns position and delete the smallest bit in number
size_t popBit(size_t &bb);

// Returns the number of bits set to true
// (we use it because it easy to construct bitset from ull)
size_t countBit(std::bitset<64> bb);

void clearBit(size_t &bb, int square);

void setBit(size_t &bb, int square);

extern std::array<size_t, regularNC> clearMask;
extern std::array<size_t, regularNC> setMask;

extern std::array<std::array<size_t, 120>, 13> pieceKeys;
extern std::array<size_t, 16> castleKeys;
extern size_t sideKey;

#endif // __BOARD_H__
