#pragma once

#include "Model/game_data.h"

struct SearchConfig {
  int maxDepth;
  int timeLimitMs;
  int beamWidth;
};

struct SearchStats {
  unsigned long long visitedNodes;
  unsigned long long prunedBranches;
  int completedDepth;
  double elapsedMs;
  int timedOut;
};

struct SearchResult {
  int row;
  int col;
  long long score;
  SearchStats stats;
};

SearchResult FindHardMove(const int board[][BOARD_SIZE], int botPiece,
                          SearchConfig config);
SearchResult GetLastHardSearchResult();
void PrintHardSearchStats(SearchResult result);
void GetHardMove(int &outRow, int &outCol);
