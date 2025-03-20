#include "move.h"

int Move::getFrom() const { return info & 0x7f; }

int Move::getTo() const { return (info >> 7) & 0x7f; }

int Move::getCaptured() const { return (info >> 14) & 0xf; }

int Move::getPromoted() const { return (info >> 20) & 0xf; }

int Move::getEnPas() const { return info & 0x40000; }

int Move::getPawnStart() const { return info & 0x80000; }

int Move::getCastle() const { return info & 0x100000; }

int Move::getPawnBits() const { return 0x7c000; }

int Move::getPromotedBits() const { return 0xf00000; }
