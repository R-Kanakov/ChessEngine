#include <bit>

#include "board.hpp"

using namespace board;

void Board::reset() {
  using cs = util::ñonsecutive_sequence<size_t, EMPTY, regularNC - 1>::type;
  using reset = util::sequence_in_shell<size_t, cs, OFFBOARD>;
  board = reset::get();

  std::fill(pawns.begin(), pawns.end(), 0ull);
  std::fill(bigPiece.begin(), bigPiece.end(), 0);
  std::fill(majPiece.begin(), majPiece.end(), 0);
  std::fill(minPiece.begin(), minPiece.end(), 0);
  std::fill(pieceNum.begin(), pieceNum.end(), 0);
  std::fill(material.begin(), material.end(), 0);

  kings[WHITE] = kings[BLACK] = 0;

  side       = BOTH;
  enPas      = NO_SQ;
  fiftyMove  = 0;
  castlePerm = 0;

  ply    = 0;
  hisPly = 0;
  posKey = 0ull;
}

void Board::parseFEN(const std::string_view &fen) {
  reset();

  size_t rank = RANK_8, file = FILE_A, piece = EMPTY;

  static std::unordered_map<char, int> pieceParser{
      {'p', bP}, {'P', wP}, {'n', bN}, {'N', wN}, {'k', bK}, {'K', wK},
      {'r', bR}, {'R', wR}, {'b', bB}, {'B', wB}, {'q', bQ}, {'Q', wQ}};

  static std::unordered_map<char, int> countParser{
      {'1', 1}, {'5', 5}, {'2', 2}, {'6', 6},
      {'3', 3}, {'7', 7}, {'4', 4}, {'8', 8}};

  auto it = fen.begin(), end = fen.end();
  for (;; ++it) {
    if (*it == '/') {
      --rank;
      file = FILE_A;
      continue;
    }

    if (*it == ' ')
      break;

    if (isdigit(*it)) {
      int count = countParser[*it];
      for (int i = 0; i < count; ++i) {
        board[convertFR(file, rank)] = EMPTY;
        file++;
      }
    } else {
      piece = pieceParser[*it];
      board[convertFR(file, rank)] = piece;
      file++;
    }
  }

  std::advance(it, 1);

  if (*it == 'w')
    side = WHITE;
  else if (*it == 'b')
    side = BLACK;

  std::advance(it, 2);

  if (*it != '-') {
    for (; it != end; ++it) {
      if (*it == 'K')
        castlePerm |= WKC;
      else if (*it == 'Q')
        castlePerm |= WQC;
      else if (*it == 'k')
        castlePerm |= BKC;
      else if (*it == 'q')
        castlePerm |= BQC;
      else if (*it == ' ')
        break;
    }
  } else
    ++it;
  
  ++it;

  if (*it != '-') {
    file = *it - 'a';
    rank = *++it - '1';
    enPas = convertFR(file, rank);
  }

  posKey = generate();

  update();
}

size_t Board::generate() const {
  size_t key = 0, piece = EMPTY;
  for (int sq = 0; sq < largeNC; ++sq) {
    piece = board[sq];
    if (piece != NO_SQ && piece != EMPTY && piece != OFFBOARD)
      key ^= pieceKeys[piece][sq];
  }

  if (side == WHITE)
    key ^= sideKey;

  if (enPas != NO_SQ)
    key ^= pieceKeys[EMPTY][enPas];

  key ^= castleKeys[castlePerm];

  return key;
}

void Board::dump(std::ostream& os) const {
  os << "\n        Game board\n";

  for (int rank = RANK_8; rank >= RANK_1; --rank) {
    os << rank + 1 << " ";
    
    for (int file = FILE_A; file <= FILE_H; ++file)
      os << std::setw(3) << pieceChar[board[convertFR(file, rank)]];
    
    os << "\n";
  }

  os << "\n  ";

  for (int file = FILE_A; file <= FILE_H; ++file)
    os << std::setw(3) << static_cast<char>('a' + file);

  os << "\nside : " << sideChar[side] << "\n";
  os << "enPas : "  << enPas << "\n";
  os << "Castle : " << (castlePerm & WKC ? 'K' : '-')
                    << (castlePerm & WQC ? 'Q' : '-')
                    << (castlePerm & BKC ? 'k' : '-')
                    << (castlePerm & BQC ? 'q' : '-');

  os << "\nposKey : " << std::hex << posKey << std::dec << "\n";
}

void Board::update() {
  for (int i = 0; i < largeNC; ++i) {
    size_t piece = board[i];
    if (piece != OFFBOARD && piece != EMPTY) {
      int color = pieceCol[piece];

      if (piece == wP) {
        setBit(pawns[WHITE], convert120To64(i));
        setBit(pawns[BOTH],  convert120To64(i));
      } else if (piece == bP) {
        setBit(pawns[BLACK], convert120To64(i));
        setBit(pawns[BOTH],  convert120To64(i));
      } else
        bigPiece[color]++;

      if (pieceMin[piece])
        minPiece[color]++;
      else if (pieceMaj[piece])
        majPiece[color]++;

      if (piece == wK)
        kings[WHITE] = i;
      else if (piece == bK)
        kings[BLACK] = i;

      material[color] += pieceVal[piece];
      pieceList[piece][pieceNum[piece]++] = i;
    }
  }
}

void Board::check() const {
  #if defined(DEBUG_ONLY)
  int t_PieceNum[13] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int t_bigPiece[2]  = {0, 0};
  int t_majPiece[2]  = {0, 0};
  int t_minPiece[2]  = {0, 0};
  int t_material[2]  = {0, 0};

  size_t sq64, t_piece, t_Piece_num, sq120, colour, pcount;

  size_t t_pawns[3] = {0ULL, 0ULL, 0ULL};

  t_pawns[WHITE] = pawns[WHITE];
  t_pawns[BLACK] = pawns[BLACK];
  t_pawns[BOTH]  = pawns[BOTH];

  // check piece lists
  for (t_piece = wP; t_piece <= bK; ++t_piece) {
    for (t_Piece_num = 0; t_Piece_num < pieceNum[t_piece]; ++t_Piece_num) {
      sq120 = pieceList[t_piece][t_Piece_num];
      assert(board[sq120] == t_piece);
    }
  }

  // Checking piece count and other counters
  for (sq64 = 0; sq64 < 64; ++sq64) {
    sq120 = convert64To120(sq64);
    t_piece = board[sq120];

    t_PieceNum[t_piece]++;
    colour = pieceCol[t_piece];

    if (pieceBig[t_piece] == true) {
      t_bigPiece[colour]++;
      if (pieceMaj[t_piece] == true)
        t_majPiece[colour]++;
      if (pieceMin[t_piece] == true)
        t_minPiece[colour]++;
    }
    t_material[colour] += pieceVal[t_piece];
  }

  for (t_piece = wP; t_piece <= bK; ++t_piece)
    assert(t_PieceNum[t_piece] == pieceNum[t_piece]);

  // Checking bitboards count
  pcount = std::popcount(t_pawns[WHITE]);
  assert(pcount == pieceNum[wP]);

  pcount = std::popcount(t_pawns[BLACK]);
  assert(pcount == pieceNum[bP]);

  pcount = std::popcount(t_pawns[BOTH]);
  assert(pcount == (pieceNum[bP] + pieceNum[wP]));

  // Checking bitboards squares
  while (t_pawns[WHITE]) {
    sq64 = popBit(t_pawns[WHITE]);
    assert(board[convert64To120(sq64)] == wP);
  }

  while (t_pawns[BLACK]) {
    sq64 = popBit(t_pawns[BLACK]);
    assert(board[convert64To120(sq64)] == bP);
  }

  while (t_pawns[BOTH]) {
    sq64 = popBit(t_pawns[BOTH]);
    assert((board[convert64To120(sq64)] == bP) ||
           (board[convert64To120(sq64)] == wP));
  }

  assert(t_material[WHITE] == material[WHITE] &&
         t_material[BLACK] == material[BLACK]);

  assert(t_minPiece[WHITE] == minPiece[WHITE] &&
         t_minPiece[BLACK] == minPiece[BLACK]);

  assert(t_majPiece[WHITE] == majPiece[WHITE] &&
         t_majPiece[BLACK] == majPiece[BLACK]);

  assert(t_bigPiece[WHITE] == bigPiece[WHITE] &&
         t_bigPiece[BLACK] == bigPiece[BLACK]);

  assert(side == WHITE || side == BLACK);
  assert(generate() == posKey);

  assert(enPas == NO_SQ || (bRanks[enPas] == RANK_6 && side == WHITE) ||
         (bRanks[enPas] == RANK_3 && side == BLACK));

  assert(board[kings[WHITE]] == wK);
  assert(board[kings[BLACK]] == bK);

  assert(castlePerm >= 0 && castlePerm <= 15);
#endif
}

bool Board::isAttacked(const int sq, const unsigned char side) const {
  // Pawns
  if (side == WHITE) {
    if (board[sq - 11] == wP || board[sq - 9] == wP)
      return true;
  } else if (board[sq + 11] == bP || board[sq + 9] == bP)
    return true;

  // Knights
  for (auto knight : knightMoves) {
    size_t piece = board[sq + knight];
    if (piece != OFFBOARD && (piece == wN || piece == bN) &&
        pieceCol[piece] == side)
      return true;
  }

  // Diagonal direction (Bishops and Queens)
  auto checkBQ = [&](int curr) -> int {
    if (board[curr] == EMPTY)
      return -1;
    if (board[curr] == OFFBOARD)
      return 0;
    if ((board[curr] == wQ || board[curr] == wB || board[curr] == bQ ||
         board[curr] == bB) &&
        pieceCol[board[curr]] == side)
      return 1;
    return 0;
  };

  int curr = sq, tmp = 11;

  while (true) {
    curr += tmp;
    int res = checkBQ(curr);

    if (res == 0) {
      curr = sq;

      if (tmp == 11)       // Change direction
        tmp = -11;
      else if (tmp == -11) // Change direction
        tmp = 9;
      else if (tmp == 9)   // Change direction
        tmp = -9;
      else
        break;
    } else if (res == 1)
      return true;
  }

  // Horizontal directions (Rooks and Queens)
  auto checkRQ = [&](int curr) -> int {
    if (board[curr] == EMPTY)
      return -1;
    if (board[curr] == OFFBOARD)
      return 0;
    if ((board[curr] == wQ || board[curr] == wR || board[curr] == bQ ||
         board[curr] == bR) &&
        pieceCol[board[curr]] == side)
      return 1;
    return 0;
  };

  curr = sq, tmp = 1;

  while (true) {
    curr += tmp;
    int res = checkRQ(curr);

    if (res == 0) {
      curr = sq;

      if (tmp == 1)       // Change direction
        tmp = -1;
      else if (tmp == -1) // Change direction
        tmp = 10;
      else if (tmp == 10) // Change direction
        tmp = -10;
      else
        break;
    } else if (res == 1)
      return true;
  }

  for (auto king : kingMoves) {
    size_t piece = board[sq + king];
    if (piece != OFFBOARD && (piece == wK || piece == bK) &&
        pieceCol[piece] == side)
      return true;
  }

  return 0;
}
