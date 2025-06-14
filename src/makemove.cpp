// Licensed after GNU GPL v3

#include <cassert>
#include <algorithm>

#include "board.hpp"
#include "valid.hpp"

using namespace board;

void Board::clearPiece(const unsigned char sq) {
  assert(isOnBoard(sq));
  check();

  unsigned char piece = getPiece(sq);
  assert(isPieceValid(piece));

  hashPiece(piece, sq);

  Color col = pieceCol[piece];
  assert(isSideValid(col));

  board[sq] = EMPTY;
  material[col] -= pieceVal[piece];

  if (pieceBig[piece]) {
    --bigPiece[col];
    if (pieceMaj[piece])
      --majPiece[col];
    else
      --minPiece[col];
  }
  else {
    clearBit(pawns[col],  convert120To64(sq));
    clearBit(pawns[BOTH], convert120To64(sq));
  }

  auto it = std::find_if(pieceList[piece].begin(), pieceList[piece].end(),
                         [&sq](auto square) { return square == sq; });
  assert(it != pieceList[piece].end());

  auto diff = std::distance(pieceList[piece].begin(), it);
  assert(0 <= diff && diff < 10);

  pieceList[piece][diff] = pieceList[piece][--pieceNum[piece]];
}

void Board::addPiece(const unsigned char sq, const unsigned char piece) {
  assert(isPieceValid(piece));
  assert(isOnBoard(sq));

  hashPiece(piece, sq);

  Color col = pieceCol[piece];
  assert(isSideValid(col));

  board[sq] = piece;
  material[col] += pieceVal[piece];

  if (pieceBig[piece]) {
    ++bigPiece[col];
    if (pieceMaj[piece])
      ++majPiece[col];
    else
      ++minPiece[col];
  }
  else {
    setBit(pawns[col],  convert120To64(sq));
    setBit(pawns[BOTH], convert120To64(sq));
  }

  pieceList[piece][pieceNum[piece]++] = sq;
}

void Board::movePiece(const unsigned char from, const unsigned char to) {
  assert(isOnBoard(from));
  assert(isOnBoard(to));

  unsigned char piece = getPiece(from);
  assert(isPieceValid(piece));

  Color col = pieceCol[piece];
  assert(isSideValid(col));

  hashPiece(piece, from);
  board[from] = EMPTY;

  hashPiece(piece, to);
  board[to] = piece;

  if (!pieceBig[piece]) { // if piece is pawn
    clearBit(pawns[col],  convert120To64(from));
    clearBit(pawns[BOTH], convert120To64(from));
    setBit(pawns[col],  convert120To64(to));
    setBit(pawns[BOTH], convert120To64(to));
  }

  auto it = std::find_if(pieceList[piece].begin(), pieceList[piece].end(),
                         [&from](auto elt) { return elt == from; });

  assert(it != pieceList[piece].end());

  auto i = std::distance(pieceList[piece].begin(), it);

  pieceList[piece][i] = to; // maybe change to it = to;????
}

void Board::takeBackMove() {
  check();

  Color original_side = side;

  --hisPly;
  --ply;

  move::Undo& last_move = history.back();
  history.pop_back();
  move::Move move = last_move.getMove();

  unsigned char from     = move.getFrom(),
                to       = move.getTo(),
                captured = move.getCaptured(),
                promoted = move.getPromoted();

  assert(isOnBoard(to));
  assert(isOnBoard(from));

  if (enPas != NO_SQ)
    hashEnPas();
  hashCastle();

  castlePerm = last_move.getCastlePerm();
  fiftyMove  = last_move.getFiftyMove();
  enPas      = last_move.getEnPas();

  if (enPas != NO_SQ)
    hashEnPas();
  hashCastle();

  side = Color(side ^ 1);
  hashSide();

  if (move.getCastle()) {
    if (to == C1)
      movePiece(D1, A1);
    else if (to == C8)
      movePiece(D8, A8);
    else if (to == G1)
      movePiece(F1, H1);
    else if (to == G8)
      movePiece(F8, H8);
    else
      assert(false);
  }
  else if (move.getEnPas())
    addPiece(to + (side == WHITE ? -10 : 10), (side == WHITE ? bP : wP));

  movePiece(to, from);

  if (pieceK[getPiece(from)])
    (side == WHITE ? kings.first : kings.second) = from;

  if (captured != EMPTY) {
    assert(isPieceValid(captured));
    addPiece(to, captured);
  }

  if (promoted != EMPTY) {
    assert(isPieceValid(promoted) && !pieceP[promoted]);
    clearPiece(from);
    addPiece(from, (pieceCol[promoted] == WHITE ? wP : bP));
  }

  check();
}

bool Board::makeMove(const move::Move& move) {
  check();

  unsigned char from     = move.getFrom(),
                to       = move.getTo(),
                captured = move.getCaptured(),
                promoted = move.getPromoted();

  assert(isOnBoard(from));
  assert(isOnBoard(to));
  assert(isPieceValid(getPiece(from)));


  history.emplace_back(move, castlePerm, enPas, fiftyMove, posKey);

  if (move.getCastle()) {
    if (to == C1)
      movePiece(A1, D1);
    else if (to == C8)
      movePiece(A8, D8);
    else if (to == G1)
      movePiece(H1, F1);
    else if (to == G8)
      movePiece(H8, F8);
    else
      assert(false);
  }
  else if (move.getEnPas())
    clearPiece(to + (side == WHITE ? -10 : 10));

  if (enPas != NO_SQ)
    hashEnPas();
  hashCastle();

  castlePerm &= bCastlePerm[to];
  castlePerm &= bCastlePerm[from];
  enPas = NO_SQ;

  hashCastle();

  ++fiftyMove;
  if (captured != EMPTY) {
    assert(isPieceValid(captured));
    clearPiece(to);
    fiftyMove = 0;
  }

  ++hisPly;
  ++ply;

  if (pieceP[getPiece(from)]) {
    fiftyMove = 0;
    if (move.getPawnStart()) {
      enPas = Cell(from + (side == WHITE ? 10 : -10));
      assert(bRanks[enPas] == (side == WHITE ? RANK_3 : RANK_6));
      hashEnPas();
    }
  }

  movePiece(from, to);

  if (promoted != EMPTY) {
    assert(isPieceValid(promoted) && !pieceP[promoted]);
    clearPiece(to);
    addPiece(to, promoted);
  }

  if (pieceK[getPiece(to)])
    (side == WHITE ? kings.first : kings.second) = to;

  side = Color(side ^ 1);
  hashSide();

  check();

  if (isAttacked(((side ^ 1) == WHITE ? kings.first : kings.second), side)) {
    takeBackMove();
    return false;
  }

  return true;
}
