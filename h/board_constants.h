#ifndef __CHESS_CONSTANTS__
#define __CHESS_CONSTANTS__

#include <array>
#include <bitset>
#include <string>

#include "util.h"

// Color
enum { WHITE, BLACK, BOTH };

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

extern std::array<std::array<size_t, 120>, 13> pieceKeys;
extern std::array<size_t, 16> castleKeys;
extern size_t sideKey;

// NC - Number of Cells
constexpr int largeNC = 120;
constexpr int regularNC = 64;

constexpr std::array<unsigned char, 64> bitTable{
    63, 30, 3,  32, 25, 41, 22, 33, 15, 50, 42, 13, 11, 53, 19, 34,
    61, 29, 2,  51, 21, 43, 45, 10, 18, 47, 1,  54, 9,  57, 0,  35,
    62, 31, 40, 4,  49, 5,  52, 26, 60, 6,  23, 44, 46, 27, 56, 16,
    7,  39, 48, 24, 59, 14, 12, 55, 38, 28, 58, 20, 37, 17, 36, 8};

// FEN start position
constexpr std::string_view startPos =
    "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// Is current piece a big piece?
constexpr std::array pieceBig{false, false, true, true, true, true, true,
                              false, true,  true, true, true, true};

// Is current piece a major piece?
constexpr std::array pieceMaj{false, false, false, false, true, true, true,
                              false, false, false, true,  true, true};

// Is current piece a minor piece?
constexpr std::array pieceMin{false, false, true, true,  false, false, false,
                              false, true,  true, false, false, false};
// Cost of each piece
// pawn = 100, knight = 325, bishop = 325,
// rook = 550, queen = 1000, king = 50000
constexpr std::array<int, 13> pieceVal{0,   100, 325, 325, 550,  1000, 50000,
                                       100, 325, 325, 550, 1000, 50000};
// What color is piece
constexpr std::array<unsigned char, 13> pieceCol{
    BOTH,  WHITE, WHITE, WHITE, WHITE, WHITE, WHITE,
    BLACK, BLACK, BLACK, BLACK, BLACK, BLACK};

// Array that give back file from square
template <size_t Start, size_t End> struct wrapped_concat_sequence {
  using type =
      typename util::concat<util::simple_sequence, Start, End, 8>::type;
};

using filesSequence =
    typename util::sequence_in_shell<wrapped_concat_sequence, FILE_A, FILE_H,
                                     OFFBOARD>;
constexpr std::array<size_t, largeNC> bFiles = filesSequence{}.get();

// Array that give back rank from square
template <size_t Start, size_t End> struct wrapped_columnar_sequence {
  using type = typename util::columnar_sequence<RANK_1, RANK_8, 8>::type;
};

using columnar_sequence_in_shell =
    util::sequence_in_shell<wrapped_columnar_sequence, RANK_1, RANK_8,
                            OFFBOARD>;
constexpr std::array<size_t, largeNC> bRanks =
    columnar_sequence_in_shell{}.get();

// Array which contains mask to set each element in [0, 63] range
using setSequence = typename util::sequence_with_shift<0, regularNC - 1>::type;
constexpr std::array<size_t, regularNC> setMask =
    util::make_array_from_sequence(setSequence{});

// Array which contains mask to clear each element in [0, 63] range
using clearSequence =
    typename util::sequence_with_tilda_shift<0, regularNC - 1>::type;
constexpr std::array<size_t, regularNC> clearMask =
    util::make_array_from_sequence(clearSequence{});

// 120 index board
using Sequence120 = util::sequence_in_shell<util::simple_sequence, 0, 63, 100>;
constexpr std::array<size_t, largeNC> Board120 = Sequence120{}.get();

// 64 index board
using Sequence64 = typename util::range_sequence_with_two_gaps<21, 98>::type;
constexpr std::array<size_t, 64> Board64 =
    util::make_array_from_sequence(Sequence64{});

// Basic conversion from [file, rank] to index of 120 squares board
inline size_t convertFR(size_t file, size_t rank) {
  return 21 + file + 10 * rank;
}

// Convert index from 120 squares board to the index of 64 squares board
inline size_t convert64To120(size_t sq) { return Board64[sq]; }

// Convert index from 64 squares board to the index of 120 squares board
inline size_t convert120To64(size_t sq) { return Board120[sq]; }

// Printing the board to the console
inline void printBitBoard(size_t bb) {
  size_t shift = 1ull;
  std::cout << "\n";
  for (int rank = RANK_8; rank >= RANK_1; --rank) {
    for (int file = FILE_A; file <= FILE_H; ++file) {
      size_t sq = convertFR(file, rank);
      size_t sq64 = convert120To64(sq);
      if ((shift << sq64) & bb)
        std::cout << "X";
      else
        std::cout << "-";
    }
    std::cout << "\n";
  }
}

// Returns position and delete the smallest bit in number
inline size_t popBit(size_t &bb) {
  size_t b = bb ^ (bb - 1);
  unsigned fold = (b & UINT32_MAX) ^ (b >> 32);
  bb &= bb - 1;
  return bitTable[static_cast<int>((fold * 0x783a9b23u) >> 26u)];
}

// Returns the number of bits set to 1
inline size_t countBit(std::bitset<64> bb) { return bb.count(); }

inline void clearBit(size_t &bb, size_t square) { bb &= clearMask[square]; }

inline void setBit(size_t &bb, size_t square) { bb |= setMask[square]; }

#endif
