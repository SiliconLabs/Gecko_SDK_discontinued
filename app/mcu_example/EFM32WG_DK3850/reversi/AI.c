/***************************************************************************//**
 * @file
 * @brief Gecko Artificial Intelligence for the game of Reversi
 * @version 5.1.2
 *******************************************************************************
 * @section License
 * <b>Copyright 2015 Silicon Labs, Inc. http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silabs License Agreement. See the file
 * "Silabs_License_Agreement.txt" for details. Before using this software for
 * any purpose, you must agree to the terms of that agreement.
 *
 ******************************************************************************/

#include "AI.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define DEPTH 4
#define END_GAME_DEPTH 9

#define INFINITY 1000000
#define WINNING_BONUS 100000
#define VALUE_OF_A_MOVE_POSSIBILITY 15
#define VALUE_OF_A_CORNER 1000
#define VALUE_OF_A_SAFE_PIECE 20
#define VALUE_OF_AN_UNSAFE_PIECE 8

static const int32_t _values[8][8] =
{ { 1000, -100, 25, 0, 0, 25, -100, 1000 },
  { -100, -400, -5, -5, -5, -5, -400, -100 },
  { 25, -5, 12, 2, 2, 12, -5, 25 },
  { 0, -5, 2, 2, 2, 2, -5, 0 },
  { 0, -5, 2, 2, 2, 2, -5, 0 },
  { 25, -5, 12, 2, 2, 12, -5, 25 },
  { -100, -400, -5, -5, -5, -5, -400, -100 },
  { 1000, -100, 25, 0, 0, 25, -100, 1000 } };

static const int _xs[60] = { 7, 7, 0, 0, 7, 7, 5, 5, 2, 2, 0, 0, 5, 5, 2, 2, 5, 5, 4, 4, 3, 3, 2, 2, 7, 7, 4, 4, 3, 3, 0, 0, 6, 6, 6, 6, 5, 5, 4, 4, 3, 3, 2, 2, 1, 1, 1, 1, 7, 7, 6, 6, 1, 1, 0, 0, 6, 6, 1, 1 };
static const int _ys[60] = { 7, 0, 7, 0, 5, 2, 7, 0, 7, 0, 5, 2, 5, 2, 5, 2, 4, 3, 5, 2, 5, 2, 4, 3, 4, 3, 7, 0, 7, 0, 4, 3, 5, 4, 3, 2, 6, 1, 6, 1, 6, 1, 6, 1, 5, 4, 3, 2, 6, 1, 7, 0, 7, 0, 6, 1, 6, 1, 6, 1 };

static BOARD _boardStack[END_GAME_DEPTH+1];
static bool _safe[10][10];
static int* _px;
static int* _py;

/**
 * Find the value of all the pieces belonging to the given player.
 * A positive value is good for this player.
 * The value can also be negative, if the player occupies tiles
 * next to a free corner, which makes it easier for the opponent
 * to get to this corner.
 */
int32_t valuePieces(BOARD* pBoard, int player) {
  int32_t sum = 0;

  for (int y = 1; y <= 8; y++) {
    for (int x = 1; x <= 8; x++) {
      _safe[x][y] = false;
    }
  }

  int corners = 0;
  corners += pBoard->aCells[0][0] == player;
  corners += pBoard->aCells[7][0] == player;
  corners += pBoard->aCells[0][7] == player;
  corners += pBoard->aCells[7][7] == player;

  if (corners) {
    /* Corners are the most valuable asset of the position. */
    sum += corners * VALUE_OF_A_CORNER;
    /* Calculate how many pieces are safe,
     * meaning they can never be taken back
     * by the opponent. */
    while (true) {
      /* Repeatedly swipe the board looking for safe pieces,
       * until no more safe pieces are found. */
      bool changed = false;
      int numberOfSafe = 0;
      for (int y = 1; y <= 8; y++) {
        for (int x = 1; x <= 8; x++) {
          if (!_safe[x][y] && pBoard->aCells[x-1][y-1] == player) {
            /* A piece can never be taken back if in each of the four directions,
             * one of the two neighbouring tiles are safe. */
            bool s = (_safe[x-1][y] || _safe[x+1][y])  /* West  - East  */
              && (_safe[x][y-1] || _safe[x][y+1])      /* North - South */
              && (_safe[x-1][y-1] || _safe[x+1][y+1])  /* NW    - SE    */
              && (_safe[x-1][y+1] || _safe[x+1][y-1]); /* SW    - NE    */
            if (s) {
              _safe[x][y] = true;
              changed = true;
              ++numberOfSafe;
            }
          }
        }
        sum += numberOfSafe * VALUE_OF_A_SAFE_PIECE;
      }
      if (!changed) break;
    }
  }

  /* Now add the value of the unsafe pieces. */
  for (int y = 0; y < 8; y++) {
    int yCorner = y<4 ? 0 : 7;
    for (int x = 0; x < 8; x++) {
      if (pBoard->aCells[x][y] == player && !_safe[x+1][y+1]) {
        int xCorner = x<4 ? 0 : 7;
        if (pBoard->aCells[xCorner][yCorner]) {
          /* If the corner is taken, we value each position in
           * the quadrant the same. */
          sum += VALUE_OF_AN_UNSAFE_PIECE;
        } else {
          /* If the corner is still free, we use a lookup table
           * to find the value of each position. */
          sum += _values[x][y];
        }
      }
    }
  }
  return sum;
}

/**
 * Evaluation function for a Reversi board.
 * Positive value means player 1 is in the lead,
 * negative value means player 2 is in the lead.
 */
int32_t eval(BOARD* pBoard) {
  int ActPlayer = pBoard->ActPlayer;
  pBoard->ActPlayer = 1;
  int movesA = _CalcValidMoves(pBoard);
  pBoard->ActPlayer = 2;
  int movesB = _CalcValidMoves(pBoard);
  pBoard->ActPlayer = ActPlayer;

  if (movesA==0 && movesB==0) {
    /* The game is over */
    pBoard->ActPlayer = 1;
    int32_t score = _CalcScore(pBoard);
    pBoard->ActPlayer = ActPlayer;
    if (score==0) {
      return 0;
    }
    if (score > 0) {
      return score + WINNING_BONUS;
    }
    if (score > 0) {
      return score - WINNING_BONUS;
    }
  }

  /* A high number of possible moves is very valuable */
  int32_t value = VALUE_OF_A_MOVE_POSSIBILITY * (movesA - movesB);

  value += valuePieces(pBoard, 1);
  value -= valuePieces(pBoard, 2);

  return value;
}

/** Minimax search for the best possible move with alpha-beta pruning */
int32_t descend(int depth, int32_t alpha, int32_t beta, bool firstMove) {
  BOARD* pBoard = _boardStack + depth;
  BOARD* nextBoard = _boardStack + depth - 1;

  if (depth == 0) {
    return eval(pBoard);
  }

  int moves = _CalcValidMoves(pBoard);
  if (moves == 0) {
      /* The player has to pass */
      pBoard->ActPlayer = 3 - pBoard->ActPlayer;
      moves = _CalcValidMoves(pBoard);
      if (moves == 0) {
        /* The game is over */
        return eval(pBoard);
      }
  }

  bool maximize = pBoard->ActPlayer == 1;
  for (int i=0; i<60; ++i) {
    /* Try the possible moves in order from most attractive to least attractive
     * position, to maximize the effect of the alpha-beta pruning. */
    int x = _xs[i];
    int y = _ys[i];
    if (pBoard->aMoves[x][y]) {
      *nextBoard = *pBoard;
      _MakeMove(nextBoard, x, y);
      nextBoard->ActPlayer = 3 - pBoard->ActPlayer;
      /* Recursively evaluate the board resulting from this move. */
      int32_t alt = descend(depth-1, alpha, beta, false);
      if (maximize) {
        if (alt > alpha) {
          alpha = alt;
          if (firstMove) {
            *_px = x;
            *_py = y;
          }
        }
      } else {
        if (alt < beta) {
          beta = alt;
          if (firstMove) {
            *_px = x;
            *_py = y;
          }
        }
      }
      if (beta <= alpha) {
        /* This is the alpha-beta pruning terminal condition. */
        goto end;
      }
    }
  }
end:
  return maximize ? alpha : beta;
}


char _PlayerAI_SmartGecko(const BOARD* pBoard, int* px, int* py) {
  /* Initialize the safe board to true to get the edges right */
  for (int y = 0; y < 10; y++) {
    for (int x = 0; x < 10; x++) {
      _safe[x][y] = true;
    }
  }
  *px = -1;
  *py = -1;
  _px = px;
  _py = py;

  int freeTiles = 0;
  for (int y = 0; y < 8; y++) {
    for (int x = 0; x < 8; x++) {
      if (!pBoard->aCells[x][y]) {
        ++freeTiles;
      }
    }
  }

  int depth = DEPTH;
  if (freeTiles <= END_GAME_DEPTH) {
    /* In the end game, we expand the search depth. */
    depth = freeTiles;
  }

  _boardStack[depth] = *pBoard;
  descend(depth, -INFINITY, INFINITY, true);
  if (*px == -1) {
    return 0;
  }
  return 1;
}
