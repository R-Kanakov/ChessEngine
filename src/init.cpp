#include <random>

#include "init.hpp"

std::array<std::array<size_t, 120>, 13> pieceKeys;
std::array<size_t, 16> castleKeys;
size_t sideKey;

void initHash() {
  std::mt19937 mersenne(std::random_device{}());
  std::uniform_int_distribution<size_t> dis(1, ULLONG_MAX);

  std::generate(castleKeys.begin(), castleKeys.end(), 
                [&d = dis, &m = mersenne]() { return d(m); });

  sideKey = dis(mersenne);

  for (int i = 0; i < 13; ++i)
    std::generate(pieceKeys[i].begin(), pieceKeys[i].end(),
                  [&d = dis, &m = mersenne]() { return d(m); });
}

void allInit() {
  initHash();
}
