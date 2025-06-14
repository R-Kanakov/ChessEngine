// Licensed after GNU GPL v3

#include "move.hpp"
#include "board.hpp"

#include <algorithm>

using namespace move;

namespace {
// Flags which will be used in `Move` construction
constexpr unsigned en_pas = 0x40000;
constexpr unsigned pawn_start = 0x80000;
constexpr unsigned castle = 0x1000000;
} // anonymous namespace

std::string Move::getDumpMove() {
#if defined(DEBUG_ONLY)
  dump_str.clear();

  unsigned char ff = bFiles[getFrom()];
  unsigned char rf = bRanks[getFrom()];

  unsigned char ft = bFiles[getTo()];
  unsigned char rt = bRanks[getTo()];

  unsigned char promoted = getPromoted();

  dump_str += 'a' + ff;
  dump_str += '1' + rf;
  dump_str += 'a' + ft;
  dump_str += '1' + rt;

  if (promoted) {
    char pchar = 'q';
    if (promoted == wN || promoted == bN)
      pchar = 'n';
    else if (promoted == wR || promoted == bR)
      pchar = 'r';
    else if (promoted == wB || promoted == bB)
      pchar = 'b';
    dump_str += pchar;
  }

  return dump_str;
#else
  return "";
#endif
}


void MoveList::dump(std::ostream& os) const {
#if defined(DEBUG_ONLY)
  os << "MoveList: \n";

  for (int i = 0; i < size(); ++i) {
    Move m = operator[](i);
    os << "Move: " << i + 1 << " " << m.getDumpMove() << ", score: " << m.getScore() << "\n";
  }
  os << "MoveList total moves: " << size();
#endif
}

void MoveList::AddQuietMove(const board::Board & b, Move && m) {
  push_back(m);
}

void MoveList::AddCapturedMove(const board::Board& b, Move&& m) {
  push_back(m);
}

void MoveList::AddEnPasMove(const board::Board& b, Move&& m) {
  push_back(m);
}

void MoveList::AddPawnMove(const board::Board& b, Move&& m, bool captured) {
  unsigned char from = m.getFrom(),
                to   = m.getTo(),
                cap  = captured ? m.getCaptured() : EMPTY;
  Color side = b.getSide();

  if (captured) {
    if (bRanks[m.getFrom()] == (side == WHITE ? RANK_7 : RANK_2)) {
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wQ : bQ)));
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wR : bR)));
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wB : bB)));
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wN : bN)));
    }
    else
      AddCapturedMove(b, Move(from, to, cap));
  }
  else {
    if (bRanks[m.getFrom()] == (side == WHITE ? RANK_7 : RANK_2)) {
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wQ : bQ)));
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wR : bR)));
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wB : bB)));
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wN : bN)));
    }
    else
      AddQuietMove(b, Move(from, to));
  }
}

void MoveList::generateAllMoves(const board::Board& b) {
  b.check();

  unsigned char piece = EMPTY;

  Color side       = b.getSide(),
        other_side = Color(side ^ 1); // WHITE == BLACK ^ 1

  char shift_9    = (side == WHITE ?      9 :     -9),
       shift_10   = (side == WHITE ?     10 :    -10),
       shift_11   = (side == WHITE ?     11 :    -11),
       shift_20   = (side == WHITE ?     20 :    -20);
  
  unsigned char
       shift_rank = (side == WHITE ? RANK_2 : RANK_7),
       shift_pawn = (side == WHITE ?     wP :     bP),
       shift_f    = (side == WHITE ?     F1 :     F8),
       shift_g    = (side == WHITE ?     G1 :     G8),
       shift_e    = (side == WHITE ?     E1 :     E8),
       shift_d    = (side == WHITE ?     D1 :     D8),
       shift_c    = (side == WHITE ?     C1 :     C8),
       shift_b    = (side == WHITE ?     B1 :     B8);

  // Pawns
  for (unsigned char piece_num = 0; piece_num < b.getPieceNum(shift_pawn); ++piece_num) {
    unsigned char sq = b.getPieceListSq(shift_pawn, piece_num);

    if (b.getPiece(sq + shift_10) == EMPTY) {
      AddPawnMove(b, Move(sq, sq + shift_10), false);
      if (bRanks[sq] == shift_rank && b.getPiece(sq + shift_20) == EMPTY)
        AddQuietMove(b, Move(sq, sq + shift_20, EMPTY, EMPTY, pawn_start));
    }

    if (bRanks[sq + shift_9] != OFFBOARD &&
        pieceCol[b.getPiece(sq + shift_9)] == other_side)
      AddPawnMove(b, Move(sq, sq + shift_9, b.getPiece(sq + shift_9)), true);

    if (bRanks[sq + shift_11] != OFFBOARD &&
        pieceCol[b.getPiece(sq + shift_11)] == other_side)
      AddPawnMove(b, Move(sq, sq + shift_11, b.getPiece(sq + shift_11)), true);

    if (b.getEnPas() != NO_SQ) {
      if (sq + shift_9 == b.getEnPas())
        AddEnPasMove(b, Move(sq, sq + shift_9, EMPTY, EMPTY, en_pas));

      if (sq + shift_11 == b.getEnPas())
        AddEnPasMove(b, Move(sq, sq + shift_11, EMPTY, EMPTY, en_pas));
    }
  }

  // Castling
  if (((b.getCastlePerm()) & (side == WHITE ? WKC : BKC)) &&
      b.getPiece(shift_f) == EMPTY &&
      b.getPiece(shift_g) == EMPTY &&
      !b.isAttacked(shift_e, other_side) &&
      !b.isAttacked(shift_f, other_side))
    AddQuietMove(b, Move(shift_e, shift_g, EMPTY, EMPTY, castle));

  if (((b.getCastlePerm()) & (side == WHITE ? WQC : BQC)) &&
      b.getPiece(shift_d) == EMPTY &&
      b.getPiece(shift_c) == EMPTY &&
      b.getPiece(shift_b) == EMPTY &&
      !b.isAttacked(shift_e, other_side) &&
      !b.isAttacked(shift_d, other_side))
    AddQuietMove(b, Move(shift_e, shift_c, EMPTY, EMPTY, castle));

  // Slide pieces
  static constexpr std::array<Piece, 3> loop_slide_piece_w{wB, wR, wQ};
  static constexpr std::array<Piece, 3> loop_slide_piece_b{bB, bR, bQ};
  
  auto loop_slide_piece = (side == WHITE ? loop_slide_piece_w : loop_slide_piece_b);

  std::for_each(loop_slide_piece.begin(), loop_slide_piece.end(), [this, &b, other_side](auto piece) {
    for (int piece_num = 0; piece_num < b.getPieceNum(piece); ++piece_num) {
      unsigned char sq = b.getPieceListSq(piece, piece_num);
      for (int i = 0; i < directionNumber[piece]; ++i) {
        char dir = pieceDirections[piece][i];
        unsigned char target_sq = sq + dir;

        for (unsigned char target_piece = b.getPiece(target_sq);
             target_piece != OFFBOARD;
             target_sq += dir, target_piece = b.getPiece(target_sq)) {
          if (target_piece != EMPTY) {
            if (pieceCol[target_piece] == other_side)
              AddCapturedMove(b, Move(sq, target_sq, b.getPiece(target_sq)));
            break;
          }
          AddQuietMove(b, Move(sq, target_sq));
        }
      }
    }
  });

  // Non-slide pieces
  static constexpr std::array<Piece, 2> loop_piece_w {wN, wK};
  static constexpr std::array<Piece, 2> loop_piece_b {bN, bK};

  auto loop_piece = (side == WHITE ? loop_piece_w : loop_piece_b);
  
  std::for_each(loop_piece.begin(), loop_piece.end(), [this, &b, other_side](auto piece) {
    for (int piece_num = 0; piece_num < b.getPieceNum(piece); ++piece_num) {
      unsigned char sq = b.getPieceListSq(piece, piece_num);

      for (int i = 0; i < directionNumber[piece]; ++i) {
        char dir = pieceDirections[piece][i];
        unsigned char target_sq = sq + dir;
        unsigned char target_piece = b.getPiece(target_sq);

        if (target_piece == OFFBOARD)
          continue;
        else if (target_piece != EMPTY) {
          if (pieceCol[target_piece] == other_side)
            AddCapturedMove(b, Move(sq, target_sq, b.getPiece(target_sq)));
        }
        else
          AddQuietMove(b, Move(sq, target_sq));
      }
    }
  });
}


char* move::getDumpSquare(const int sq) {
  static char SqStr[2];

  int file = bFiles[sq];
  int rank = bRanks[sq];

  SqStr[0] = 'a' + file;
  SqStr[1] = '1' + rank;

  return SqStr;
}