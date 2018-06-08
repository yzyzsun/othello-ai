//
// Created by Sun Yaozhu on 2018-06-06
// Copyright (c) 2018 Sun Yaozhu. All rights reserved.
//

#ifndef OTHELLO_H_
#define OTHELLO_H_

#include <cassert>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

class OthelloBoard {
 public:
  enum Player : char { kBlack = 'B', kWhite = 'W', kUnknown = '0' };

  OthelloBoard();
  OthelloBoard(std::string board, Player player);
  OthelloBoard(const OthelloBoard& other);
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
    Iterator() : othello_(nullptr) {}
    explicit Iterator(const OthelloBoard* othello)
        : othello_(othello) { ++(*this); }
    Iterator& operator++() {
      Inc();
      while (othello_ && !othello_->CanPlay(row_, col_)) Inc();
      return *this;
    }
    bool operator!=(Iterator other) const {
      return othello_ != other.othello_;
    }
    OthelloBoard operator*() const {
      assert(othello_ != nullptr);
      return othello_->Play(row_, col_);
    }
   protected:
    void Inc() {
      if (othello_ == nullptr) return;
      ++col_;
      if (col_ == 8) {
        col_ = 0;
        ++row_;
        if (row_ == 8) othello_ = nullptr;
      }
    }
   private:
    const OthelloBoard* othello_;
    int row_ = 0;
    int col_ = -1;
  };
  Iterator begin() const { return Iterator(this); }
  Iterator end() const { return Iterator(); }

 protected:
  bool IsInside(int row, int col) const;
  bool CanFlip(int row, int col, int delta_row, int delta_col) const;
  char Square(int row, int col) const;

 private:
  Player board_[8][8];
  Player self_;
  Player oppo_;
  int last_row_ = -1;
  int last_col_ = -1;
};

#endif  // OTHELLO_H_
