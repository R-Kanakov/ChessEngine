#include "init.h"

std::array<int, largeNC> Board120;

std::array<size_t, regularNC> setMask;
std::array<size_t, regularNC> clearMask;

std::array<std::array<size_t, 120>, 13> pieceKeys;
std::array<size_t, 16> castleKeys;
size_t sideKey;

void initBoards() { // TODO: remove it
  std::fill(Board120.begin(), Board120.end(), regularNC + 1);

  for (int rank = RANK_1, sq = A1, sq64 = 0; rank <= RANK_8; ++rank) {
    for (int file = FILE_A; file <= FILE_H; ++file, ++sq64) {
      sq = convertFR(file, rank);
      Board120[sq] = sq64;
    }
  }
}

void initBitMasks() {
  for (int i = 0; i < regularNC; ++i) {
    setMask[i] = 0 | (1ull << i);
    clearMask[i] = ~setMask[i];
  }
}

void initHash() {
  std::random_device rd;
  std::mt19937 mersenne(rd());
  std::uniform_int_distribution<size_t> dis(1, ULLONG_MAX);

  std::generate(castleKeys.begin(), castleKeys.end(), 
                [&d = dis, &m = mersenne]() { return d(m); });

  sideKey = dis(mersenne);

  for (int i = 0; i < 13; ++i)
    std::generate(pieceKeys[i].begin(), pieceKeys[i].end(),
                  [&d = dis, &m = mersenne]() { return d(m); });
}

void allInit() {
  initBoards();
  initBitMasks();
  initHash();
}
