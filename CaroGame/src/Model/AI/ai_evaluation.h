#pragma once

#include "Model/game_data.h"

enum PatternType {
  WIN,
  OPEN_4,
  BLOCKED_4,
  OPEN_3,
  BLOCKED_3,
  OPEN_2,
  BLOCKED_2,
  OPEN_1,
  BLOCKED_1,
  OVERLINE,
  PATTERN_NONE
};

struct LinePattern {
  int count;
  int blockedEnds;
  int gaps;
  int patternType;
};

constexpr long long HARD_SCORES[] = {
    1000000000LL, 100000000LL, 1000000LL, 10000000LL, 10000LL,
    100000LL,     100LL,       10LL,      1LL,        -50000000LL};

constexpr long long MEDIUM_SCORES[] = {
    1000000000LL, 10000000LL, 5000000LL, 10000LL, 1000LL,
    100LL,        10LL,       2LL,       1LL,     -5000000LL};

constexpr long long EASY_SCORES[] = {
    1000000000LL, 10000000LL, 5000000LL, 10LL, 5LL,
    1LL,          0LL,        0LL,       0LL,  -1000000LL};

PatternType ClassifyLine(int count, int blocks, int gaps);

LinePattern AnalyzeLine(const int board[][BOARD_SIZE], int row, int col,
                        int piece, int deltaRow, int deltaCol, int allowGap);

long long EvaluatePositionWithScores(const int board[][BOARD_SIZE], int row,
                                     int col, int piece,
                                     const long long scores[], int allowGap,
                                     int isBoardEval);

long long EvaluatePositionEasy(int row, int col, int piece);
long long EvaluatePositionEasyOnBoard(int row, int col, int piece,
                                      const int board[][BOARD_SIZE]);
long long EvaluatePositionMedium(int row, int col, int piece,
                                 const int board[][BOARD_SIZE]);
long long EvaluatePositionHard(int row, int col, int piece,
                               const int board[][BOARD_SIZE],
                               bool isBoardEval = false);
