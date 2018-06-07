//
// Created by Sun Yaozhu on 2018-06-06
// Copyright (c) 2018 Sun Yaozhu. All rights reserved.
//

#include <cstdlib>
#include <limits>
#include <memory>
#include "othello.h"

using namespace std;

static constexpr auto kMaxInt = numeric_limits<int>::max();

int main(int argc, char* argv[]) {
  if (argc != 2 && argc != 3) {
    cout << "Othello AI using negamax search with alpha-beta pruning"
         << endl << endl;
    cout << "USAGE: othello mode [depth]" << endl;
    cout << "MODE:" << endl;
    cout << "  00    Both humans" << endl;
    cout << "  01    Black human, White AI" << endl;
    cout << "  10    Black AI, White human" << endl;
    cout << "  11    Both AIs" << endl;
    cout << "DEPTH: default value is 10" << endl;
    return 0;
  }
  auto black_is_ai = argv[1][0] == '1';
  auto white_is_ai = argv[1][1] == '1';
  auto depth = 10;
  if (argc == 3) depth = atoi(argv[2]);

  cout << "Welcome to the game of Othello!" << endl;
  auto othello = OthelloBoard();
  auto pass_times = 0;
  while (pass_times < 2) {
    cout << endl;
    othello.Print();
    auto t = othello.WhoseTurn();
    cout << endl << (t == 'B' ? "Black" : "White") << "'s turn "
         << "(" << (t == 'B' ? "*" : "O") << ")" << endl;
    if (!othello.CanPlay()) {
      cout << "= PASS" << endl;
      othello = othello.Pass();
      ++pass_times;
      continue;
    }
    if ((t == 'B' && black_is_ai) || (t == 'W' && white_is_ai)) {
      auto node = othello;
      auto best = -kMaxInt;
      for (auto child : node) {
        auto score = -child.Negamax(depth, -kMaxInt, kMaxInt);
        if (score >= best) {
          best = score;
          othello = child;
        }
      }
      auto move = othello.LastMove();
      cout << "= " << move.first << " " << move.second << endl;
    } else {
      int row = -1; int col = -1;
      while (!othello.CanPlay(row, col)) {
        cout << "> ";
        cin >> row >> col;
      }
      othello = othello.Play(row, col);
    }
    pass_times = 0;
  }
  cout << endl << "Game Over!" << endl;
  auto score = othello.AbsoluteScore();
  if (score == 0) {
    cout << "Draw" << endl;
  } else if ((score > 0) == (othello.WhoseTurn() == 'B')) {
    cout << "Black wins" << endl;
  } else {
    cout << "White wins" << endl;
  }
  return 0;
}
