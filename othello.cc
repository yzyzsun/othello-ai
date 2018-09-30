//
// Created by Sun Yaozhu on 2018-06-06
// Copyright (c) 2018 Sun Yaozhu. All rights reserved.
//

#include "othello.h"
#include <algorithm>
#include <limits>

using namespace std;

static constexpr auto kMaxInt = numeric_limits<int>::max();

OthelloBoard::OthelloBoard()
    : self_(kBlack), oppo_(kWhite) {
  board_[kBlack] = 0x0000000810000000;
  board_[kWhite] = 0x0000001008000000;
  GenerateMoves();
}

OthelloBoard::OthelloBoard(string board, OthelloBoard::Player player) {
  board_[kBlack] = board_[kWhite] = 0;
  for (auto i = 0; i < 64; ++i) {
    switch (board[2*i]) {
    case 'B': Set(i / 8, i % 8, kBlack);
    case 'W': Set(i / 8, i % 8, kWhite);
    }
  }
  if (player == kUnknown) {
    self_ = oppo_ = kUnknown;
    moves_ = 0;
    return;
  }
  self_ = player;
  oppo_ = self_ == kBlack ? kWhite : kBlack;
  GenerateMoves();
}

OthelloBoard::Player OthelloBoard::WhoseTurn() const {
  return self_;
}

bool OthelloBoard::IsInside(int row, int col) const {
  if (row < 0 || row >= 8 || col < 0 || col >= 8) return false;
  return true;
}

OthelloBoard::Player OthelloBoard::Get(int row, int col) const {
  assert(IsInside(row, col));

  uint64_t mask = 1ULL << (8 * row + col);
  if (board_[kBlack] & mask) return kBlack;
  if (board_[kWhite] & mask) return kWhite;
  return kUnknown;
}

void OthelloBoard::Set(int row, int col, Player player) {
  assert(IsInside(row, col));
  assert(player == kBlack || player == kWhite);

  uint64_t mask = 1ULL << (8 * row + col);
  board_[player] |= mask;
}

// Adapted from https://www.hanshq.net/othello.html
uint64_t OthelloBoard::Shift(uint64_t board, int dir) const {
  static constexpr int kShifts[] = {
     1,  // Right
     9,  // Down-Right
     8,  // Down
     7,  // Down-Left
    -1,  // Left
    -9,  // Up-Left
    -8,  // Up
    -7   // Up-Right
  };
  static constexpr uint64_t kMasks[] = {
    0xFEFEFEFEFEFEFEFE,  // Right
    0xFEFEFEFEFEFEFE00,  // Down-Right
    0xFFFFFFFFFFFFFF00,  // Down
    0x7F7F7F7F7F7F7F00,  // Down-Left
    0x7F7F7F7F7F7F7F7F,  // Left
    0x007F7F7F7F7F7F7F,  // Up-Left
    0x00FFFFFFFFFFFFFF,  // Up
    0x00FEFEFEFEFEFEFE   // Up-Right
  };

  assert(0 <= dir && dir < 8);
  if (kShifts[dir] > 0) return (board << kShifts[dir]) & kMasks[dir];
  return (board >> -kShifts[dir]) & kMasks[dir];
}

void OthelloBoard::GenerateMoves() {
  moves_ = 0;
  uint64_t empty = ~(board_[kBlack] | board_[kWhite]);
  for (auto dir = 0; dir < 8; ++dir) {
    uint64_t t = Shift(board_[self_], dir) & board_[oppo_];
    for (auto _ = 0; _ < 5; ++_) t |= Shift(t, dir) & board_[oppo_];
    moves_ |= Shift(t, dir) & empty;
  }
}

bool OthelloBoard::CanPlay(int row, int col) const {
  if (!IsInside(row, col)) return false;
  uint64_t mask = 1ULL << (8 * row + col);
  return moves_ & mask;
}

bool OthelloBoard::CanPlay() const {
  return moves_;
}

bool OthelloBoard::IsOver() const {
  return last_row_ == -1 && !CanPlay();
}

OthelloBoard OthelloBoard::Pass() const {
  auto new_board = *this;
  new_board.self_ = oppo_;
  new_board.oppo_ = self_;
  new_board.last_row_ = -1;
  new_board.last_col_ = -1;
  new_board.GenerateMoves();
  return new_board;
}

OthelloBoard OthelloBoard::Play(int row, int col) const {
  assert(OthelloBoard::CanPlay(row, col));

  auto new_board = *this;
  new_board.self_ = oppo_;
  new_board.oppo_ = self_;
  new_board.last_row_ = row;
  new_board.last_col_ = col;
  new_board.Set(row, col, self_);

  uint64_t flip = 0;
  uint64_t pos = 1ULL << (8 * row + col);
  for (auto dir = 0; dir < 8; ++dir) {
    uint64_t t = Shift(pos, dir) & board_[oppo_];
    for (auto _ = 0; _ < 5; ++_) t |= Shift(t, dir) & board_[oppo_];
    if (Shift(t, dir) & board_[self_]) flip |= t;
  }
  new_board.board_[self_] ^= flip;
  new_board.board_[oppo_] ^= flip;

  new_board.GenerateMoves();
  return new_board;
}

int OthelloBoard::AbsoluteScore() const {
  auto score = 0;
  uint64_t mask = 1;
  for (auto _ = 0; _ < 64; ++_) {
    if (board_[self_] & mask) {
      ++score;
    } else if (board_[oppo_] & mask) {
      --score;
    }
    mask <<= 1;
  }
  return score;
}

int OthelloBoard::WeightedScore() const {
  static constexpr int kWeights[] = {
    120, -20,  20,   5,   5,  20, -20, 120,
    -20, -40,  -5,  -5,  -5,  -5, -40, -20,
     20,  -5,  15,   3,   3,  15,  -5,  20,
      5,  -5,   3,   3,   3,   3,  -5,   5,
      5,  -5,   3,   3,   3,   3,  -5,   5,
     20,  -5,  15,   3,   3,  15,  -5,  20,
    -20, -40,  -5,  -5,  -5,  -5, -40, -20,
    120, -20,  20,   5,   5,  20, -20, 120
  };

  auto score = 0;
  uint64_t mask = 1;
  for (auto i = 0; i < 64; ++i) {
    if (board_[self_] & mask) {
      score += kWeights[i];
    } else if (board_[oppo_] & mask) {
      score -= kWeights[i];
    }
    mask <<= 1;
  }
  return score;
}

int OthelloBoard::Negamax(int depth, int alpha, int beta) const {
  if (IsOver()) {
    auto score = AbsoluteScore();
    if (score == 0) return 0;
    return score > 0 ? kMaxInt : -kMaxInt;
  }
  if (depth == 0) return WeightedScore();
  if (!CanPlay()) return -Pass().Negamax(depth - 1, -beta, -alpha);
  auto best = -kMaxInt;
  for (auto child : *this) {
    auto score = -child.Negamax(depth - 1, -beta, -alpha);
    best = max(best, score);
    alpha = max(alpha, score);
    if (alpha >= beta) break;
  }
  return best;
}

pair<int, int> OthelloBoard::LastMove() const {
  return make_pair(last_row_, last_col_);
}

char OthelloBoard::Repr(int row, int col) const {
  switch (Get(row, col)) {
  case kBlack:
    return '*';
  case kWhite:
    return 'O';
  default:
    if (CanPlay(row, col)) return '.';
    return ' ';
  }
}

void OthelloBoard::PrintBoard(ostream& out) const {
  out << "  0 1 2 3 4 5 6 7" << endl;
  for (auto i = 0; i < 8; ++i) {
    out << i;
    for (auto j = 0; j < 8; ++j) {
      out << " " << Repr(i, j);
    }
    out << endl;
  }
}

void OthelloBoard::PrintResult(ostream& out) const {
  auto black = 0; auto white = 0;
  for (auto i = 0; i < 8; ++i) {
    for (auto j = 0; j < 8; ++j) {
      switch (Get(i, j)) {
      case kBlack:
        ++black;
        break;
      case kWhite:
        ++white;
        break;
      default:
        break;
      }
    }
  }
  if (black > white) {
    out << "Black wins";
  } else if (white > black) {
    out << "White wins";
  } else {
    out << "Draw";
  }
  out << " (" << black << " : " << white << ")" << endl;
}
