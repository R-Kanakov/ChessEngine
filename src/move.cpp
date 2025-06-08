// Licensed after GNU GPL v3

#include "move.hpp"
#include "board.hpp"

using namespace move;

// Flags (it will be used in `flag` argument in Move constructor)
static constexpr unsigned en_pas     = 0x40000;
static constexpr unsigned pawn_start = 0x80000;
static constexpr unsigned castle     = 0x1000000;

std::string Move::getDumpMove() {
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
}


void MoveList::dump(std::ostream& os) const {
  os << "MoveList: \n";

  for (int i = 0; i < count; ++i) {
    Move m = operator[](i);
    os << "Move: " << i + 1 << " " << m.getDumpMove() << ", score: " << m.getScore() << "\n";
  }
  os << "MoveList total moves: " << count;
}

void MoveList::AddQuietMove(const board::Board & b, Move && m) {
  push_back(m);;
}

void MoveList::AddCapturedMove(const board::Board& b, Move&& m) {
  push_back(m);
}

void MoveList::AddEnPasMove(const board::Board& b, Move&& m) {
  push_back(m);;
}

void MoveList::AddPawnMove(const board::Board& b, Move&& m, Color side, bool captured) {
  unsigned char from = m.getFrom();
  unsigned char to = m.getTo();
  unsigned char cap = captured ? m.getCaptured() : EMPTY;

  if (captured) {
    if (bRanks[m.getFrom()] == (side == WHITE ? RANK_7 : RANK_2)) {
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wQ : bQ)));
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wR : bR)));
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wB : bB)));
      AddCapturedMove(b, Move(from, to, cap, (side == WHITE ? wN : bN)));
    }
    else
      AddCapturedMove(b, Move(from, to, cap, EMPTY, 0));
  }
  else {
    if (bRanks[m.getFrom()] == (side == WHITE ? RANK_7 : RANK_2)) {
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wQ : bQ)));
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wR : bR)));
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wB : bB)));
      AddQuietMove(b, Move(from, to, cap, (side == WHITE ? wN : bN)));
    }
    else {
      AddQuietMove(b, Move(from, to, cap, EMPTY, 0));
    }
  }
}

void MoveList::GenerateAllMoves(const board::Board& b) {
  b.check();

  unsigned char piece = EMPTY;
  Color side = b.getSide();

  // Pawns
  for (int piece_num = 0; piece_num < b.getPieceNum(side == WHITE ? wP : bP); ++piece_num) {
    unsigned char sq = b.getPieceListSq(side == WHITE ? wP : bP, piece_num);

    if (b.getPiece(sq + (side == WHITE ? 10 : -10)) == EMPTY) {
      AddPawnMove(b, Move(sq, sq + (side == WHITE ? 10 : -10)), side, false);
      if (bRanks[sq] == (side == WHITE ? RANK_2 : RANK_7) &&
          b.getPiece(sq + (side == WHITE ? 20 : -20)) == EMPTY)
        AddQuietMove(b, Move(sq, sq + (side == WHITE ? 20 : -20), EMPTY, EMPTY, pawn_start));
    }

    if (board120[sq + (side == WHITE ? 9 : -9)] != OFFBOARD &&
        pieceCol[b.getPiece(sq + (side == WHITE ? 9 : -9))] == ((side ^ 1) == false ? WHITE : BLACK))
      AddPawnMove(b, Move(sq, sq + (side == WHITE ? 9: -9),
                          b.getPiece(sq + (side == WHITE ? 9 : -9))),
                  side, true);

    if (board120[sq + (side == WHITE ? 11 : -11)] != OFFBOARD &&
        pieceCol[b.getPiece(sq + ((side == WHITE ? 11 : -11)))] == ((side ^ 1) == false ? WHITE : BLACK))
      AddPawnMove(b, Move(sq, sq + (side == WHITE ? 11 : -11),
                  b.getPiece(sq + (side == WHITE ? 11 : -11))),
                  side, true);

    if (b.getEnPas() != NO_SQ) {
      if (sq + (side == WHITE ? 9 : -9) == b.getEnPas())
        AddEnPasMove(b, Move(sq, sq + (side == WHITE ? 9 : -9), EMPTY, EMPTY, en_pas));

      if (sq + (side == WHITE ? 11 : -11) == b.getEnPas())
        AddEnPasMove(b, Move(sq, sq + (side == WHITE ? 11 : -11), EMPTY, EMPTY, en_pas));
    }
  }

  // Castling
  if ((b.getCastlePerm() & side == WHITE ? WKC : BKC) &&
      b.getPiece((side == WHITE ? F1 : F8)) == EMPTY &&
      b.getPiece((side == WHITE ? G1 : G8)) == EMPTY &&
      !b.isAttacked((side == WHITE ? E1 : E8), ((side ^ 1) == false ? WHITE : BLACK)) &&
      !b.isAttacked((side == WHITE ? F1 : F8), ((side ^ 1) == false ? WHITE : BLACK)))
    AddQuietMove(b, Move((side == WHITE ? E1 : E8), (side == WHITE ? G1 : G8), EMPTY, EMPTY, castle));
    
  if ((b.getCastlePerm() & side == WHITE ? WQC : BQC) &&
      b.getPiece((side == WHITE ? D1 : D8)) == EMPTY &&
      b.getPiece((side == WHITE ? C1 : C8)) == EMPTY &&
      b.getPiece((side == WHITE ? B1 : B8)) == EMPTY &&
      !b.isAttacked((side == WHITE ? D1 : D8), ((side ^ 1) == false ? WHITE : BLACK)) &&
      !b.isAttacked((side == WHITE ? C1 : C8), ((side ^ 1) == false ? WHITE : BLACK)) &&
      !b.isAttacked((side == WHITE ? B1 : B8), ((side ^ 1) == false ? WHITE : BLACK)))
    AddQuietMove(b, Move((side == WHITE ? E1 : E8), (side == WHITE ? C1 : C8), EMPTY, EMPTY, castle));

  // Slide pieces
  static constexpr std::array<unsigned char, 8> loop_slide_piece{wB, wR, wQ, EMPTY,
                                                                 bB, bR, bQ, EMPTY};
  static constexpr std::array<unsigned char, 2> loop_slide_index{0, 4};
  
  for (unsigned char piece_index = loop_slide_index[side], piece = loop_slide_piece[piece_index];
       piece != EMPTY;
       ++piece_index, piece = loop_slide_piece[piece_index]) {
    for (int piece_num = 0; piece_num < b.getPieceNum(piece); ++piece_num) {
      unsigned char sq = b.getPieceListSq(piece, piece_num);

      for (int i = 0; i < directionNumber[piece]; ++i) {
        char dir = pieceDirections[piece][i];
        unsigned char target_sq = sq + dir;

        while (bFiles[target_sq] != OFFBOARD) {
          size_t target_piece = b.getPiece(target_sq);
          if (target_piece != EMPTY) {
            if (pieceCol[target_piece] == (side ^ 1))
              AddCapturedMove(b, Move(sq, target_sq, b.getPiece(target_sq)));
            break;
          }
          AddQuietMove(b, Move(sq, target_sq));
          target_sq += dir;
        }
      }
    }
  }

  // Non-slide pieces
  static constexpr std::array<unsigned char, 6> loop_piece{wN, wK, EMPTY,
                                                 bN, bK, EMPTY};
  static constexpr std::array<unsigned char, 2> loop_index{0, 3};

  for (int piece_index = loop_index[side], piece = loop_piece[piece_index];
       piece != EMPTY;
       ++piece_index, piece = loop_piece[piece_index]) {
    for (int piece_num = 0; piece_num < b.getPieceNum(piece); ++piece_num) {
      int sq = b.getPieceListSq(piece, piece_num);

      for (int i = 0; i < directionNumber[piece]; ++i) {
        int dir = pieceDirections[piece][i];
        int target_sq = sq + dir;

        size_t target_piece = b.getPiece(target_sq);

        if (target_piece == OFFBOARD)
          continue;
        else if (target_piece != EMPTY) {
          if (pieceCol[target_piece] == (side ^ 1))
            AddCapturedMove(b, Move(sq, target_sq, b.getPiece(target_sq)));
        }
        else
          AddQuietMove(b, Move(sq, target_sq));
      }
    }
  }
}


char* move::getDumpSquare(const int sq) {
  static char SqStr[2];

  int file = bFiles[sq];
  int rank = bRanks[sq];

  SqStr[0] = 'a' + file;
  SqStr[1] = '1' + rank;

  return SqStr;
}