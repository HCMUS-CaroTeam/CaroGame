#include "ai_evaluation.h"

static int IsInsideBoard(int row, int col) {
  return row >= 0 && row < BOARD_SIZE && col >= 0 && col < BOARD_SIZE;
}

PatternType ClassifyLine(int count, int blocks, int gaps) {
  if (blocks >= 2)
    return PATTERN_NONE;

  if (gaps > 0) {
    if (count >= WIN_LENGTH)
      count = WIN_LENGTH - 1;
  } else {
    if (count > WIN_LENGTH)
      return OVERLINE;
    if (count == WIN_LENGTH)
      return WIN;
  }

  if (blocks == 0) {
    if (count == 4)
      return OPEN_4;
    if (count == 3)
      return OPEN_3;
    if (count == 2)
      return OPEN_2;
    if (count == 1)
      return OPEN_1;
  } else {
    if (count == 4)
      return BLOCKED_4;
    if (count == 3)
      return BLOCKED_3;
    if (count == 2)
      return BLOCKED_2;
    if (count == 1)
      return BLOCKED_1;
  }

  return PATTERN_NONE;
}

static void ScanLineSide(const int board[][BOARD_SIZE], int row, int col,
                         int piece, int deltaRow, int deltaCol, int allowGap,
                         int *count, int *blockedEnds, int *gaps) {
  int usedGap = 0;
  for (int step = 1;; step++) {
    const int nextRow = row + step * deltaRow;
    const int nextCol = col + step * deltaCol;

    if (!IsInsideBoard(nextRow, nextCol)) {
      (*blockedEnds)++;
      return;
    }

    if (board[nextRow][nextCol] == piece) {
      (*count)++;
      continue;
    }

    if (board[nextRow][nextCol] != CELL_EMPTY) {
      (*blockedEnds)++;
      return;
    }

    const int afterRow = nextRow + deltaRow;
    const int afterCol = nextCol + deltaCol;
    if (allowGap && !usedGap && IsInsideBoard(afterRow, afterCol) &&
        board[afterRow][afterCol] == piece) {
      usedGap = 1;
      (*gaps)++;
      continue;
    }

    return;
  }
}

LinePattern AnalyzeLine(const int board[][BOARD_SIZE], int row, int col,
                        int piece, int deltaRow, int deltaCol, int allowGap) {
  LinePattern pattern = {1, 0, 0, PATTERN_NONE};
  ScanLineSide(board, row, col, piece, deltaRow, deltaCol, allowGap,
               &pattern.count, &pattern.blockedEnds, &pattern.gaps);
  ScanLineSide(board, row, col, piece, -deltaRow, -deltaCol, allowGap,
               &pattern.count, &pattern.blockedEnds, &pattern.gaps);
  pattern.patternType =
      ClassifyLine(pattern.count, pattern.blockedEnds, pattern.gaps);
  return pattern;
}

long long EvaluatePositionWithScores(const int board[][BOARD_SIZE], int row,
                                     int col, int piece,
                                     const long long scores[], int allowGap,
                                     int isBoardEval) {
  static const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
  long long totalScore = 0;

  for (int i = 0; i < 4; i++) {
    const int deltaRow = directions[i][0];
    const int deltaCol = directions[i][1];

    if (isBoardEval) {
      const int previousRow = row - deltaRow;
      const int previousCol = col - deltaCol;
      if (IsInsideBoard(previousRow, previousCol) &&
          board[previousRow][previousCol] == piece)
        continue;
      const int previous2Row = row - 2 * deltaRow;
      const int previous2Col = col - 2 * deltaCol;
      if (IsInsideBoard(previousRow, previousCol) &&
          board[previousRow][previousCol] == CELL_EMPTY &&
          IsInsideBoard(previous2Row, previous2Col) &&
          board[previous2Row][previous2Col] == piece)
        continue;
    }

    LinePattern pattern =
        AnalyzeLine(board, row, col, piece, deltaRow, deltaCol, allowGap);
    if (pattern.patternType != PATTERN_NONE)
      totalScore += scores[pattern.patternType];
  }

  return totalScore;
}

long long EvaluatePositionEasyOnBoard(int row, int col, int piece,
                                      const int board[][BOARD_SIZE]) {
  return EvaluatePositionWithScores(board, row, col, piece, EASY_SCORES, 0, 0);
}

long long EvaluatePositionEasy(int row, int col, int piece) {
  return EvaluatePositionEasyOnBoard(row, col, piece, current().board);
}

long long EvaluatePositionMedium(int row, int col, int piece,
                                 const int board[][BOARD_SIZE]) {
  return EvaluatePositionWithScores(board, row, col, piece, MEDIUM_SCORES, 0,
                                    0);
}

long long EvaluatePositionHard(int row, int col, int piece,
                               const int board[][BOARD_SIZE],
                               bool isBoardEval) {
  return EvaluatePositionWithScores(board, row, col, piece, HARD_SCORES, 1,
                                    isBoardEval ? 1 : 0);
}
