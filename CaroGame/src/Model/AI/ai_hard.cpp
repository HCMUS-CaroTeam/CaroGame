#include "ai_hard.h"

#include "ai_evaluation.h"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstring>

static constexpr long long SEARCH_INF = 10000000000000000LL;
typedef int ScratchBoard[BOARD_SIZE][BOARD_SIZE];

struct SearchMove {
  int row;
  int col;
  long long score;
};

struct SearchContext {
  long long deadlineNs;
  int timedOut;
  SearchConfig config;
  SearchStats *stats;
};

static SearchResult lastHardSearchResult = {};

static long long GetNowNs() {
  return std::chrono::duration_cast<std::chrono::nanoseconds>(
             std::chrono::steady_clock::now().time_since_epoch())
      .count();
}

static int IsSearchTimedOut(SearchContext *context) {
  if (context->timedOut)
    return 1;
  if (GetNowNs() < context->deadlineNs)
    return 0;
  context->timedOut = 1;
  context->stats->timedOut = 1;
  return 1;
}

static int CountPieces(const ScratchBoard board) {
  int count = 0;
  for (int row = 0; row < BOARD_SIZE; row++)
    for (int col = 0; col < BOARD_SIZE; col++)
      if (board[row][col] != CELL_EMPTY)
        count++;
  return count;
}

static void FindFirstOccupiedCell(const ScratchBoard board, int *outRow,
                                  int *outCol) {
  *outRow = -1;
  *outCol = -1;
  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int col = 0; col < BOARD_SIZE; col++) {
      if (board[row][col] != CELL_EMPTY) {
        *outRow = row;
        *outCol = col;
        return;
      }
    }
  }
}

static int GetSearchRadius(const ScratchBoard board) {
  const int pieceCount = CountPieces(board);
  if (pieceCount <= 6)
    return 3;
  if (pieceCount <= 20)
    return 2;
  return 1;
}

static int GetBeamWidth(int depth, int totalMoves, SearchConfig config) {
  if (config.beamWidth > 0)
    return std::min(totalMoves, config.beamWidth);
  if (depth >= 4)
    return std::min(totalMoves, 15);
  if (depth >= 2)
    return std::min(totalMoves, 20);
  return std::min(totalMoves, 25);
}

static int HasNeighbor(const ScratchBoard board, int row, int col, int radius) {
  for (int deltaRow = -radius; deltaRow <= radius; deltaRow++) {
    for (int deltaCol = -radius; deltaCol <= radius; deltaCol++) {
      const int nextRow = row + deltaRow;
      const int nextCol = col + deltaCol;
      if (nextRow >= 0 && nextRow < BOARD_SIZE && nextCol >= 0 &&
          nextCol < BOARD_SIZE && board[nextRow][nextCol] != CELL_EMPTY)
        return 1;
    }
  }
  return 0;
}

static bool CompareSearchMoves(const SearchMove &left,
                               const SearchMove &right) {
  return left.score > right.score;
}

static int GeneratePotentialMoves(SearchMove moves[], int botPiece,
                                  int playerPiece, ScratchBoard board,
                                  int depth, SearchConfig config) {
  const int radius = GetSearchRadius(board);
  const int pieceCount = CountPieces(board);
  const int isRootDepth = depth == config.maxDepth;
  int count = 0;

  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int col = 0; col < BOARD_SIZE; col++) {
      if (board[row][col] != CELL_EMPTY ||
          !HasNeighbor(board, row, col, radius))
        continue;

      const long long attack =
          EvaluatePositionHard(row, col, botPiece, board);
      const long long defense =
          EvaluatePositionHard(row, col, playerPiece, board);
      long long score = attack + defense;
      if (isRootDepth && pieceCount <= 20)
        score = attack + defense * 2;
      moves[count++] = {row, col, score};
    }
  }

  std::sort(moves, moves + count, CompareSearchMoves);
  return GetBeamWidth(depth, count, config);
}

static long long EvaluateBoard(const ScratchBoard board, int botPiece,
                               int playerPiece) {
  long long totalScore = 0;
  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int col = 0; col < BOARD_SIZE; col++) {
      if (board[row][col] == botPiece) {
        const long long score =
            EvaluatePositionHard(row, col, botPiece, board, true);
        if (score >= HARD_SCORES[WIN])
          return SEARCH_INF / 2;
        totalScore += score;
      } else if (board[row][col] == playerPiece) {
        const long long score =
            EvaluatePositionHard(row, col, playerPiece, board, true);
        if (score >= HARD_SCORES[WIN])
          return -SEARCH_INF / 2;
        totalScore -= score * 3 / 2;
      }
    }
  }
  return totalScore;
}

static long long AlphaBeta(ScratchBoard board, int depth, long long alpha,
                           long long beta, int maximizing, int botPiece,
                           int playerPiece, SearchContext *context) {
  context->stats->visitedNodes++;
  if (IsSearchTimedOut(context))
    return 0;
  if (depth == 0)
    return EvaluateBoard(board, botPiece, playerPiece);

  SearchMove moves[BOARD_SIZE * BOARD_SIZE];
  const int moveCount = GeneratePotentialMoves(
      moves, botPiece, playerPiece, board, depth, context->config);
  if (moveCount == 0)
    return EvaluateBoard(board, botPiece, playerPiece);

  if (maximizing) {
    long long best = -SEARCH_INF;
    for (int i = 0; i < moveCount; i++) {
      const int row = moves[i].row;
      const int col = moves[i].col;
      board[row][col] = botPiece;

      long long score;
      if (EvaluatePositionHard(row, col, botPiece, board) >= HARD_SCORES[WIN])
        score = SEARCH_INF / 2 + depth;
      else
        score = AlphaBeta(board, depth - 1, alpha, beta, 0, botPiece,
                          playerPiece, context);
      board[row][col] = CELL_EMPTY;

      if (context->timedOut)
        return 0;
      best = std::max(best, score);
      alpha = std::max(alpha, score);
      if (beta <= alpha) {
        context->stats->prunedBranches += moveCount - i - 1;
        break;
      }
    }
    return best;
  }

  long long best = SEARCH_INF;
  for (int i = 0; i < moveCount; i++) {
    const int row = moves[i].row;
    const int col = moves[i].col;
    board[row][col] = playerPiece;

    long long score;
    if (EvaluatePositionHard(row, col, playerPiece, board) >= HARD_SCORES[WIN])
      score = -SEARCH_INF / 2 - depth;
    else
      score = AlphaBeta(board, depth - 1, alpha, beta, 1, botPiece,
                        playerPiece, context);
    board[row][col] = CELL_EMPTY;

    if (context->timedOut)
      return 0;
    best = std::min(best, score);
    beta = std::min(beta, score);
    if (beta <= alpha) {
      context->stats->prunedBranches += moveCount - i - 1;
      break;
    }
  }
  return best;
}

static SearchResult MakeImmediateResult(int row, int col, long long score,
                                        long long startNs) {
  SearchResult result = {};
  result.row = row;
  result.col = col;
  result.score = score;
  result.stats.elapsedMs = (GetNowNs() - startNs) / 1000000.0;
  return result;
}

static int FindOpeningMove(const ScratchBoard board, int botPiece, int *outRow,
                           int *outCol) {
  if (botPiece != CELL_O || CountPieces(board) != 1)
    return 0;

  int enemyRow = -1;
  int enemyCol = -1;
  FindFirstOccupiedCell(board, &enemyRow, &enemyCol);
  if (enemyRow < 0 || enemyCol < 0)
    return 0;

  static const int preferredOffsets[8][2] = {
      {-1, -1}, {-1, 1}, {1, -1}, {1, 1},
      {-1, 0},  {0, -1}, {0, 1},  {1, 0},
  };

  for (int i = 0; i < 8; i++) {
    const int row = enemyRow + preferredOffsets[i][0];
    const int col = enemyCol + preferredOffsets[i][1];
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
      continue;
    if (board[row][col] == CELL_EMPTY) {
      *outRow = row;
      *outCol = col;
      return 1;
    }
  }

  return 0;
}

static int FindImmediateTacticalMove(const ScratchBoard board, int piece,
                                     int *outRow, int *outCol) {
  ScratchBoard scratch;
  std::memcpy(scratch, board, sizeof(scratch));

  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int col = 0; col < BOARD_SIZE; col++) {
      if (scratch[row][col] != CELL_EMPTY)
        continue;
      scratch[row][col] = piece;
      const long long score = EvaluatePositionHard(row, col, piece, scratch);
      scratch[row][col] = CELL_EMPTY;
      if (score >= HARD_SCORES[WIN]) {
        *outRow = row;
        *outCol = col;
        return 1;
      }
    }
  }

  return 0;
}

SearchResult FindHardMove(const int board[][BOARD_SIZE], int botPiece,
                          SearchConfig config) {
  const long long startNs = GetNowNs();
  if (config.maxDepth < 1)
    config.maxDepth = 1;
  if (config.timeLimitMs < 1)
    config.timeLimitMs = 1;

  SearchResult result = {};
  result.row = -1;
  result.col = -1;
  result.score = -SEARCH_INF;

  const int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;
  ScratchBoard scratch;
  std::memcpy(scratch, board, sizeof(scratch));

  int openingRow = -1;
  int openingCol = -1;
  if (FindOpeningMove(scratch, botPiece, &openingRow, &openingCol))
    return MakeImmediateResult(openingRow, openingCol, SEARCH_INF / 4,
                               startNs);

  int tacticalRow = -1;
  int tacticalCol = -1;
  if (FindImmediateTacticalMove(scratch, botPiece, &tacticalRow, &tacticalCol))
    return MakeImmediateResult(tacticalRow, tacticalCol, SEARCH_INF / 2,
                               startNs);
  if (FindImmediateTacticalMove(scratch, playerPiece, &tacticalRow,
                                &tacticalCol))
    return MakeImmediateResult(tacticalRow, tacticalCol, SEARCH_INF / 2 - 1,
                               startNs);

  SearchMove rootMoves[BOARD_SIZE * BOARD_SIZE];
  const int rootMoveCount = GeneratePotentialMoves(
      rootMoves, botPiece, playerPiece, scratch, config.maxDepth, config);

  if (rootMoveCount == 0) {
    const int center = BOARD_SIZE / 2;
    if (scratch[center][center] == CELL_EMPTY) {
      result.row = center;
      result.col = center;
      result.score = 0;
    }
    result.stats.elapsedMs = (GetNowNs() - startNs) / 1000000.0;
    return result;
  }

  SearchContext context = {};
  context.deadlineNs = startNs + (long long)config.timeLimitMs * 1000000LL;
  context.config = config;
  context.stats = &result.stats;

  for (int targetDepth = 1; targetDepth <= config.maxDepth; targetDepth++) {
    long long iterationBestScore = -SEARCH_INF;
    int iterationBestRow = -1;
    int iterationBestCol = -1;
    long long alpha = -SEARCH_INF;

    for (int i = 0; i < rootMoveCount; i++) {
      if (IsSearchTimedOut(&context))
        break;

      const int row = rootMoves[i].row;
      const int col = rootMoves[i].col;
      scratch[row][col] = botPiece;
      const long long score =
          AlphaBeta(scratch, targetDepth - 1, alpha, SEARCH_INF, 0, botPiece,
                    playerPiece, &context);
      scratch[row][col] = CELL_EMPTY;

      if (context.timedOut)
        break;
      if (score > iterationBestScore) {
        iterationBestScore = score;
        iterationBestRow = row;
        iterationBestCol = col;
      }
      alpha = std::max(alpha, score);
    }

    if (context.timedOut)
      break;
    result.row = iterationBestRow;
    result.col = iterationBestCol;
    result.score = iterationBestScore;
    result.stats.completedDepth = targetDepth;
  }

  if (result.row < 0) {
    result.row = rootMoves[0].row;
    result.col = rootMoves[0].col;
    result.score = rootMoves[0].score;
  }
  result.stats.elapsedMs = (GetNowNs() - startNs) / 1000000.0;
  return result;
}

SearchResult GetLastHardSearchResult() { return lastHardSearchResult; }

void PrintHardSearchStats(SearchResult result) {
  std::printf("\n[HARD AI SEARCH]\n");
  std::printf("Move: (%d, %d)  Score: %lld\n", result.row, result.col,
              result.score);
  std::printf("Depth: %d  Nodes: %llu  Pruned: %llu\n",
              result.stats.completedDepth, result.stats.visitedNodes,
              result.stats.prunedBranches);
  std::printf("Time: %.2f ms  Timeout: %s\n", result.stats.elapsedMs,
              result.stats.timedOut ? "yes" : "no");
}

void GetHardMove(int &outRow, int &outCol) {
  SearchConfig config = {5, 300, 0};
  lastHardSearchResult =
      FindHardMove(current().board, current().turn, config);
  outRow = lastHardSearchResult.row;
  outCol = lastHardSearchResult.col;
  PrintHardSearchStats(lastHardSearchResult);
}
