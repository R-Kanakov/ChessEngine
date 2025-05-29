#include "move.hpp"
#include "board.hpp"

using namespace move;

Move::Move(int from, int to, int captured, int promoted, int flag) :
  info(from | (to << 7) | (captured << 14) | (promoted << 20) | flag), score(0) {}

int Move::getFrom() const { return info & 0x7f; }

int Move::getTo() const { return (info >> 7) & 0x7f; }

int Move::getCaptured() const { return (info >> 14) & 0xf; }

int Move::getPromoted() const { return (info >> 20) & 0xf; }

int Move::getEnPas() const { return info & 0x40000; }

int Move::getPawnStart() const { return info & 0x80000; }

int Move::getCastle() const { return info & 0x100000; }

int Move::getPawnBits() const { return info & 0x7c000; }

int Move::getPromotedBits() const { return info & 0xf00000; }

std::string Move::getDumpMove() {
  dump_str.clear();

  int ff = bFiles[getFrom()];
  int rf = bRanks[getFrom()];

  int ft = bFiles[getTo()];
  int rt = bRanks[getTo()];

  int promoted = getPromoted();

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


char* move::getDumpSquare(const int sq) {
  static char SqStr[2];

  int file = bFiles[sq];
  int rank = bRanks[sq];

  SqStr[0] = 'a' + file;
  SqStr[1] = '1' + rank;

  return SqStr;
}


void MoveList::dump(std::ostream& os) {
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
  int from = m.getFrom();
  int to = m.getTo();
  int cap = captured ? m.getCaptured() : EMPTY;

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
  // list->count = 0
  b.check();

  int piece = EMPTY;
  Color side = b.getSide();

  // Pawns
  if (side == WHITE) {
    for (int piece_num = 0; piece_num < b.getPieceNum(wP); ++piece_num) {
      int sq = b.getPieceListSq(wP, piece_num);

      if (b.getPiece(sq + 10) == EMPTY) {
        AddPawnMove(b, Move(sq, sq + 10), WHITE, false);
        if (bRanks[sq] == RANK_2 && b.getPiece(sq + 20) == EMPTY)
          AddQuietMove(b, Move(sq, sq + 20, EMPTY, EMPTY, pawn_start));
      }

      if (Board120[sq + 9] != OFFBOARD && pieceCol[b.getPiece(sq + 9)] == BLACK)
        AddPawnMove(b, Move(sq, sq + 9, b.getPiece(sq + 9)), WHITE, true);

      if (Board120[sq + 11] != OFFBOARD && pieceCol[b.getPiece(sq + 11)] == BLACK)
        AddPawnMove(b, Move(sq, sq + 11, b.getPiece(sq + 11)), WHITE, true);

      if (b.getEnPas() != NO_SQ) {
        if (sq + 9 == b.getEnPas())
          AddEnPasMove(b, Move(sq, sq + 9, EMPTY, EMPTY, en_pas));

        if (sq + 11 == b.getEnPas())
          AddEnPasMove(b, Move(sq, sq + 11, EMPTY, EMPTY, en_pas));
      }
    }

    if ((b.getCastlePerm() & WKC) && 
        (b.getPiece(F1) == EMPTY && b.getPiece(G1) == EMPTY) &&
        (!b.isAttacked(E1, BLACK) && !b.isAttacked(F1, BLACK)))
      AddQuietMove(b, Move(E1, G1, EMPTY, EMPTY, castle));
    
    if ((b.getCastlePerm() & WQC) &&
        (b.getPiece(D1) == EMPTY && b.getPiece(C1) == EMPTY && b.getPiece(B1) == EMPTY) &&
        (!b.isAttacked(D1, BLACK) && !b.isAttacked(C1, BLACK) && !b.isAttacked(B1, BLACK)))
      AddQuietMove(b, Move(E1, C1, EMPTY, EMPTY, castle));
  }
  else {
    for (int pieceNum = 0; pieceNum < b.getPieceNum(bP); ++pieceNum) {
      int sq = b.getPieceListSq(bP, pieceNum);

      if (b.getPiece(sq - 10) == EMPTY) {
        AddPawnMove(b, Move(sq, sq - 10), BLACK, false);
        if (bRanks[sq] == RANK_7 && b.getPiece(sq - 20) == EMPTY)
          AddQuietMove(b, Move(sq, sq - 20, EMPTY, EMPTY, pawn_start));
      }

      if (Board120[sq - 9] != OFFBOARD && pieceCol[b.getPiece(sq - 9)] == WHITE)
        AddPawnMove(b, Move(sq, sq - 9, b.getPiece(sq - 9)), BLACK, true);

      if (Board120[sq - 11] != OFFBOARD && pieceCol[b.getPiece(sq - 11)] == WHITE)
        AddPawnMove(b, Move(sq, sq - 11, b.getPiece(sq - 11)), BLACK, true);

      if (b.getEnPas() != NO_SQ) {
        if (sq - 9 == b.getEnPas())
          AddEnPasMove(b, Move(sq, sq - 9, EMPTY, EMPTY, en_pas));

        if (sq - 11 == b.getEnPas())
          AddEnPasMove(b, Move(sq, sq - 11, EMPTY, EMPTY, en_pas));
      }
    }
    
    if ((b.getCastlePerm() & BKC) &&
        (b.getPiece(F8) == EMPTY && b.getPiece(G8) == EMPTY) &&
        (!b.isAttacked(E8, WHITE) && !b.isAttacked(F8, WHITE)))
      AddQuietMove(b, Move(E8, G8, EMPTY, EMPTY, castle));

    if ((b.getCastlePerm() & BQC) &&
        (b.getPiece(D8 == EMPTY) && b.getPiece(C8) == EMPTY && b.getPiece(B8) == EMPTY) &&
        (!b.isAttacked(D8, WHITE) && !b.isAttacked(C8, WHITE) && !b.isAttacked(B8, WHITE)))
      AddQuietMove(b, Move(E8, C8, EMPTY, EMPTY, castle));
  }

  // Slide pieces
  static constexpr std::array<int, 8> loop_slide_piece{wB, wR, wQ, EMPTY,
                                                       bB, bR, bQ, EMPTY};
  static constexpr std::array<int, 2> loop_slide_index{0, 4};
  
  for (int piece_index = loop_slide_index[side], piece = loop_slide_piece[piece_index];
       piece != EMPTY;
       ++piece_index, piece = loop_slide_piece[piece_index]) {
    for (int piece_num = 0; piece_num < b.getPieceNum(piece); ++piece_num) {
      int sq = b.getPieceListSq(piece, piece_num);

      for (int i = 0; i < directionNumber[piece]; ++i) {
        int dir = pieceDirections[piece][i];
        int target_sq = sq + dir;

        while (bFiles[target_sq] != OFFBOARD) {
          int target_piece = b.getPiece(target_sq);
          if (target_piece != EMPTY) {
            if (pieceCol[target_piece] == (side ^ 1)) // WHITE ^ 1 == BLACK
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
  static constexpr std::array<int, 6> loop_piece{wN, wK, EMPTY,
                                                 bN, bK, EMPTY};
  static constexpr std::array<int, 2> loop_index{0, 3};

  for (int piece_index = loop_index[side], piece = loop_piece[piece_index];
       piece != EMPTY;
       ++piece_index, piece = loop_piece[piece_index]) {
    for (int piece_num = 0; piece_num < b.getPieceNum(piece); ++piece_num) {
      int sq = b.getPieceListSq(piece, piece_num);

      for (int i = 0; i < directionNumber[piece]; ++i) {
        int dir = pieceDirections[piece][i];
        int target_sq = sq + dir;

        int target_piece = b.getPiece(target_sq);

        if (target_piece == OFFBOARD)
          continue;
        else if (target_piece != EMPTY) {
          if (pieceCol[target_piece] == (side ^ 1)) // WHITE ^ 1 == BLACK
            AddCapturedMove(b, Move(sq, target_sq, b.getPiece(target_sq)));
        }
        else
          AddQuietMove(b, Move(sq, target_sq));
      }
    }
  }
}
