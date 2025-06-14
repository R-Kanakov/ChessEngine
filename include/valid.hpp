// Licensed after GNU GPL v3

#if !defined(__VALID_HPP__) && defined(DEBUG_ONLY)
#define __VALID_HPP__
#include "board_constants.hpp"

bool isOnBoard(const unsigned char sq) {
  return bFiles[sq] == OFFBOARD ? false : true;
}

bool isSideValid(const Color side) {
  return (side == WHITE || side == BLACK) ? true : false;
}

bool isFileRankValid(const unsigned char fr) {
  return (fr >= 0 && fr <= 7) ? true : false;
}

bool isPieceValidOrEmpty(const unsigned char piece) {
  return (piece >= EMPTY && piece <= bK) ? true : false;
}

bool isPieceValid(const unsigned char piece) {
  return (piece >= wP && piece <= bK) ? true : false;
}

#endif // __VALID_HPP__