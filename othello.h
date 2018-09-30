//
// Created by Sun Yaozhu on 2018-06-06
// Copyright (c) 2018 Sun Yaozhu. All rights reserved.
//

#ifndef OTHELLO_H_
#define OTHELLO_H_

#include <cassert>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

class OthelloBoard {
 public:
  enum Player { kBlack = 0, kWhite = 1, kUnknown = 2 };

  OthelloBoard();
  OthelloBoard(std::string board, Player player);
  Player WhoseTurn() const;
  bool CanPlay(int row, int col) const;
  bool CanPlay() const;
  bool IsOver() const;
  OthelloBoard Play(int row, int col) const;
  OthelloBoard Pass() const;
  int AbsoluteScore() const;
  int WeightedScore() const;
  int Negamax(int depth, int alpha, int beta) const;
  std::pair<int, int> LastMove() const;
  void PrintBoard(std::ostream& out) const;
  void PrintResult(std::ostream& out) const;

  class Iterator : public std::iterator<std::input_iterator_tag, OthelloBoard> {
   public:
    explicit Iterator(const OthelloBoard* othello)
        : othello_(othello), moves_(othello->moves_) {}
    Iterator(const OthelloBoard* othello, std::uint64_t moves)
        : othello_(othello), moves_(moves) {}
    Iterator& operator++() {
      // Find first set and then minus it
      moves_ -= moves_ & -moves_;
      return *this;
    }
    bool operator!=(Iterator other) const {
      return othello_ != other.othello_ || moves_ != other.moves_;
    }
    OthelloBoard operator*() const {
      // Can be replaced by `__builtin_ffs` in GCC
      assert(moves_);
      auto index = 0;
      std::uint64_t mask = 1;
      while ((moves_ & mask) == 0) {
        ++index;
        mask <<= 1;
      }
      return othello_->Play(index / 8, index % 8);
    }
   private:
    const OthelloBoard* othello_;
    std::uint64_t moves_;
  };
  Iterator begin() const { return Iterator(this); }
  Iterator end() const { return Iterator(this, 0); }

 protected:
  bool IsInside(int row, int col) const;
  Player Get(int row, int col) const;
  void Set(int row, int col, Player player);
  std::uint64_t Shift(std::uint64_t board, int dir) const;
  void GenerateMoves();
  char Repr(int row, int col) const;

 private:
  std::uint64_t board_[2];
  std::uint64_t moves_;
  Player self_;
  Player oppo_;
  int last_row_ = -1;
  int last_col_ = -1;
};

#endif  // OTHELLO_H_
