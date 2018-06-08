//
// Created by Sun Yaozhu on 2018-06-06
// Copyright (c) 2018 Sun Yaozhu. All rights reserved.
//

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <limits>
#include <memory>
#include "othello.h"

using namespace std;

static constexpr auto kMaxInt = numeric_limits<int>::max();

int main(int argc, char* argv[]) {
  if (argc != 3) {
    cout << "Othello AI using negamax search with alpha-beta pruning"
         << endl << endl;
    cout << "USAGE: ./othello <black_option> <white_option>" << endl;
    cout << "OPTION:" << endl;
    cout << "  >=0    depth of the game tree" << endl;
    cout << "   -1    human instead of AI" << endl;
    return 0;
  }
  auto black_depth = atoi(argv[1]);
  auto white_depth = atoi(argv[2]);

  cout << "Welcome to the game of Othello!" << endl;
  auto othello = OthelloBoard();
  auto pass_times = 0;
  while (pass_times < 2) {
    cout << endl;
    othello.PrintBoard(cout);
    auto is_black = othello.WhoseTurn() == OthelloBoard::kBlack;
    cout << endl << (is_black ? "Black" : "White") << "'s turn "
         << "(" << (is_black ? "*" : "O") << ")" << endl;
    if (!othello.CanPlay()) {
      cout << "= PASS" << endl;
      othello = othello.Pass();
      ++pass_times;
      continue;
    }
    if ((is_black && black_depth >= 0) || (!is_black && white_depth >= 0)) {
      cout << "AI is thinking... "; cout.flush();
      auto start = chrono::steady_clock::now();
      auto node = othello;
      auto best = -kMaxInt;
      for (auto child : node) {
        auto score = -child.Negamax(is_black ? black_depth : white_depth,
                                    -kMaxInt, kMaxInt);
        if (score >= best) {
          best = score;
          othello = child;
        }
      }
      auto end = chrono::steady_clock::now();
      chrono::duration<double> elapsed_seconds = end - start;
      cout << elapsed_seconds.count() << "s" << endl;
      auto move = othello.LastMove();
      cout << "= " << move.first << " " << move.second << endl;
    } else {
      int row = -kMaxInt; int col = -kMaxInt;
      while (!othello.CanPlay(row, col)) {
        if (row != -kMaxInt) cout << "Cannot play at this position!" << endl;
        cout << "> ";
        cin >> row >> col;
      }
      othello = othello.Play(row, col);
    }
    pass_times = 0;
  }
  cout << endl << "Game over!" << endl;
  othello.PrintResult(cout);
  return 0;
}
