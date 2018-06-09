//
// Created by Sun Yaozhu on 2018-06-07
// Copyright (c) 2018 Sun Yaozhu. All rights reserved.
//

#include <cstdlib>
#include <limits>
#include <string>
#include "othello.h"

using namespace std;

static constexpr auto kMaxInt = numeric_limits<int>::max();

int main(int argc, char* argv[]) {
  auto depth = 10;
  if (argc == 2) depth = atoi(argv[1]);

  char player; string board;
  cin >> player >> board;

  auto othello = OthelloBoard(board, static_cast<OthelloBoard::Player>(player));
  if (!othello.CanPlay()) return 1;
  auto best = -kMaxInt;
  pair<int, int> move;
  for (auto child : othello) {
    auto score = -child.Negamax(depth, -kMaxInt, kMaxInt);
    if (score >= best) {
      best = score;
      move = child.LastMove();
    }
  }
  cout << move.first << " " << move.second << endl;
  return 0;
}
