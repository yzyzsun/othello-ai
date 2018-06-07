//
// Created by Sun Yaozhu on 2018-06-06
// Copyright (c) 2018 Sun Yaozhu. All rights reserved.
//

#include "othello.h"
#include <cassert>
#include <cstring>
#include <algorithm>
#include <limits>
#include <memory>

using namespace std;

static constexpr int kWeights[8][8] = {
  {120, -20,  20,   5,   5,  20, -20, 120},
  {-20, -40,  -5,  -5,  -5,  -5, -40, -20},
  { 20,  -5,  15,   3,   3,  15,  -5,  20},
  {  5,  -5,   3,   3,   3,   3,  -5,   5},
  {  5,  -5,   3,   3,   3,   3,  -5,   5},
  { 20,  -5,  15,   3,   3,  15,  -5,  20},
  {-20, -40,  -5,  -5,  -5,  -5, -40, -20},
  {120, -20,  20,   5,   5,  20, -20, 120}
};
static constexpr auto kMaxInt = numeric_limits<int>::max();

OthelloBoard::OthelloBoard()
    : self_('B'), oppo_('W') {
  memset(board_, '0', sizeof board_);
  board_[3][4] = board_[4][3] = 'B';
  board_[3][3] = board_[4][4] = 'W';
}

OthelloBoard::OthelloBoard(string board, char player) {
  for (auto i = 0; i < 8; ++i) {
    for (auto j = 0; j < 8; ++j) {
      board_[i][j] = board[16*i + 2*j];
    }
  }
  self_ = player;
  oppo_ = (player == 'B' ? 'W' : 'B');
}

OthelloBoard::OthelloBoard(const OthelloBoard& other)
    : self_(other.self_), oppo_(other.oppo_) {
  memcpy(board_, other.board_, sizeof board_);
}

char OthelloBoard::WhoseTurn() const {
  return self_;
}

bool OthelloBoard::IsInside(int row, int col) const {
  if (row < 0 || row >= 8 || col < 0 || col >= 8) return false;
  return true;
}

bool OthelloBoard::CanFlip(int row, int col,
                           int delta_row, int delta_col) const {
  auto i = row + delta_row;
  auto j = col + delta_col;
  auto has_oppo = false;
  while (IsInside(i, j) && board_[i][j] == oppo_) {
    i += delta_row;
    j += delta_col;
    has_oppo = true;
  }
  if (!IsInside(i, j)) return false;
  if (!has_oppo || board_[i][j] != self_) return false;
  return true;
}

bool OthelloBoard::CanPlay(int row, int col) const {
  if (!IsInside(row, col)) return false;
  if (board_[row][col] != '0') return false;
  for (auto delta_row = -1; delta_row <= 1; ++delta_row) {
    for (auto delta_col = -1; delta_col <= 1; ++delta_col) {
      if (delta_row == 0 && delta_col == 0) continue;
      if (CanFlip(row, col, delta_row, delta_col)) return true;
    }
  }
  return false;
}

bool OthelloBoard::CanPlay() const {
  for (auto i = 0; i < 8; ++i) {
    for (auto j = 0; j < 8; ++j) {
      if (CanPlay(i, j)) return true;
    }
  }
  return false;
}

bool OthelloBoard::IsOver() const {
  return last_row_ == -1 && !CanPlay();
}

OthelloBoard OthelloBoard::Pass() const {
  auto new_board = OthelloBoard(*this);
  new_board.self_ = oppo_;
  new_board.oppo_ = self_;
  return new_board;
}

OthelloBoard OthelloBoard::Play(int row, int col) const {
  assert(OthelloBoard::CanPlay(row, col));
  auto new_board = Pass();
  new_board.board_[row][col] = self_;
  new_board.last_row_ = row;
  new_board.last_col_ = col;
  for (auto delta_row = -1; delta_row <= 1; ++delta_row) {
    for (auto delta_col = -1; delta_col <= 1; ++delta_col) {
      if (delta_row == 0 && delta_col == 0) continue;
      if (CanFlip(row, col, delta_row, delta_col)) {
        auto i = row + delta_row;
        auto j = col + delta_col;
        while (board_[i][j] == oppo_) {
          new_board.board_[i][j] = self_;
          i += delta_row;
          j += delta_col;
        }
      }
    }
  }
  return new_board;
}

int OthelloBoard::AbsoluteScore() const {
  auto score = 0;
  for (auto i = 0; i < 8; ++i) {
    for (auto j = 0; j < 8; ++j) {
      if (board_[i][j] == self_) {
        ++score;
      } else if (board_[i][j] == oppo_) {
        --score;
      }
    }
  }
  return score;
}

int OthelloBoard::WeightedScore() const {
  auto score = 0;
  for (auto i = 0; i < 8; ++i) {
    for (auto j = 0; j < 8; ++j) {
      if (board_[i][j] == self_) {
        score += kWeights[i][j];
      } else if (board_[i][j] == oppo_) {
        score -= kWeights[i][j];
      }
    }
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

char OthelloBoard::Square(int row, int col) const {
  if (!IsInside(row, col)) return '!';
  switch (board_[row][col]) {
  case 'B':
    return '*';
  case 'W':
    return 'O';
  default:
    if (CanPlay(row, col)) return '.';
    return ' ';
  }
}

void OthelloBoard::Print() const {
  cout << "  0 1 2 3 4 5 6 7" << endl;
  for (auto i = 0; i < 8; ++i) {
    cout << i;
    for (auto j = 0; j < 8; ++j) {
      cout << " " << Square(i, j);
    }
    cout << endl;
  }
}
