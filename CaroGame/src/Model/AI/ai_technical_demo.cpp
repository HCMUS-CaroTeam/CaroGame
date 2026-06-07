#include "ai_technical_demo.h"

#include "Model/AI/ai_evaluation.h"
#include "Model/AI/ai_hard.h"
#include "Model/logic.h"
#include <cstdio>
#include <cstring>

static void ClearGame(DataGame *game) {
  std::memset(game, 0, sizeof(DataGame));
  game->turn = CELL_X;
  game->result = RESULT_ONGOING;
  game->lastMoveRow = -1;
  game->lastMoveCol = -1;
}

static int ReportTest(const char *name, int passed) {
  std::printf("[%s] %s\n", passed ? "PASS" : "FAIL", name);
  return passed ? 0 : 1;
}

static int TestApplyMove() {
  DataGame game;
  ClearGame(&game);
  GameMove move = {7, 7, CELL_X};
  GameMoveResult first = ApplyMove(&game, move);
  GameMoveResult second = ApplyMove(&game, move);
  return first.accepted && first.placedPiece == CELL_X && !second.accepted &&
         game.lastMoveRow == 7 && game.lastMoveCol == 7;
}

static int TestExactFive() {
  DataGame game;
  ClearGame(&game);
  for (int col = 3; col <= 7; col++)
    game.board[7][col] = CELL_X;
  GameWinInfo info = CheckWinOnBoard(game.board, 7, 7, 1);
  return info.result == RESULT_X_WINS && info.lineCount == WIN_LENGTH;
}

static int TestOverline() {
  DataGame game;
  ClearGame(&game);
  for (int col = 3; col <= 8; col++)
    game.board[7][col] = CELL_X;
  return CheckWinOnBoard(game.board, 7, 8, 1).result == RESULT_ONGOING;
}

static int TestBlockedRule() {
  DataGame game;
  ClearGame(&game);
  game.board[7][2] = CELL_O;
  game.board[7][8] = CELL_O;
  for (int col = 3; col <= 7; col++)
    game.board[7][col] = CELL_X;
  const int tournamentResult =
      CheckWinOnBoard(game.board, 7, 7, 1).result;
  const int classicResult = CheckWinOnBoard(game.board, 7, 7, 0).result;
  return tournamentResult == RESULT_ONGOING &&
         classicResult == RESULT_X_WINS;
}

static int TestPatternAnalyzer() {
  DataGame game;
  ClearGame(&game);
  game.board[7][6] = CELL_X;
  game.board[7][8] = CELL_X;
  LinePattern pattern = AnalyzeLine(game.board, 7, 7, CELL_X, 0, 1, 0);
  return pattern.count == 3 && pattern.blockedEnds == 0 &&
         pattern.patternType == OPEN_3;
}

static int TestHardImmediateWin() {
  DataGame game;
  ClearGame(&game);
  for (int col = 4; col <= 7; col++)
    game.board[7][col] = CELL_O;

  int original[BOARD_SIZE][BOARD_SIZE];
  std::memcpy(original, game.board, sizeof(original));
  SearchConfig config = {4, 100, 12};
  SearchResult result = FindHardMove(game.board, CELL_O, config);

  const int boardUnchanged =
      std::memcmp(original, game.board, sizeof(original)) == 0;
  if (result.row < 0 || result.col < 0)
    return 0;

  game.board[result.row][result.col] = CELL_O;
  const int wins =
      CheckWinOnBoard(game.board, result.row, result.col, 1).result ==
      RESULT_O_WINS;
  return boardUnchanged && wins;
}

static int TestSearchMetrics() {
  DataGame game;
  ClearGame(&game);
  game.board[7][7] = CELL_X;
  game.board[7][8] = CELL_O;
  game.board[8][7] = CELL_X;
  game.board[6][8] = CELL_O;

  SearchConfig config = {3, 100, 10};
  SearchResult result = FindHardMove(game.board, CELL_O, config);
  PrintHardSearchStats(result);
  return IsValidMove(game.board, result.row, result.col) &&
         result.stats.visitedNodes > 0 &&
         result.stats.completedDepth >= 1 &&
         result.stats.elapsedMs >= 0.0;
}

int RunLogicAITechnicalTests() {
  int failures = 0;
  std::printf("\n=== LOGIC / AI TECHNICAL TEST SUITE ===\n");
  failures += ReportTest("ApplyMove rejects occupied cells", TestApplyMove());
  failures += ReportTest("Exact five wins", TestExactFive());
  failures += ReportTest("Overline does not win", TestOverline());
  failures += ReportTest("Tournament blocked rule differs from Classic",
                         TestBlockedRule());
  failures +=
      ReportTest("Shared analyzer detects OPEN_3", TestPatternAnalyzer());
  failures += ReportTest("Hard AI wins immediately without mutating input",
                         TestHardImmediateWin());
  failures +=
      ReportTest("Iterative deepening exposes search metrics",
                 TestSearchMetrics());
  std::printf("Result: %d passed, %d failed\n", 7 - failures, failures);
  return failures == 0 ? 0 : 1;
}

static long long CompactHeatValue(long long score) {
  if (score >= HARD_SCORES[WIN])
    return 999;
  if (score >= HARD_SCORES[OPEN_4])
    return 400;
  if (score >= HARD_SCORES[OPEN_3])
    return 300;
  if (score >= HARD_SCORES[BLOCKED_4])
    return 240;
  if (score >= HARD_SCORES[OPEN_2])
    return 200;
  if (score > 0)
    return score > 99 ? 99 : score;
  return score < -99 ? -99 : score;
}

void PrintHeuristicHeatmapDemo() {
  DataGame game;
  ClearGame(&game);
  game.board[7][5] = CELL_X;
  game.board[7][6] = CELL_X;
  game.board[7][8] = CELL_X;
  game.board[6][7] = CELL_O;
  game.board[8][7] = CELL_O;

  std::printf("\n=== HEURISTIC HEATMAP (X attack + O defense) ===\n");
  std::printf("999=WIN, 400=OPEN_4, 300=OPEN_3, 240=BLOCKED_4\n\n");
  for (int row = 4; row <= 10; row++) {
    for (int col = 3; col <= 11; col++) {
      if (game.board[row][col] == CELL_X) {
        std::printf("   X ");
      } else if (game.board[row][col] == CELL_O) {
        std::printf("   O ");
      } else {
        const long long attack =
            EvaluatePositionHard(row, col, CELL_X, game.board);
        const long long defense =
            EvaluatePositionHard(row, col, CELL_O, game.board);
        std::printf("%4lld ", CompactHeatValue(attack + defense));
      }
    }
    std::printf("\n");
  }
}

static int HasAnyPiece(const int board[][BOARD_SIZE]) {
  for (int row = 0; row < BOARD_SIZE; row++)
    for (int col = 0; col < BOARD_SIZE; col++)
      if (board[row][col] != CELL_EMPTY)
        return 1;
  return 0;
}

static GameMove FindGreedyMove(const int board[][BOARD_SIZE], int piece) {
  GameMove best = {-1, -1, piece};
  long long bestScore = -1;
  const int opponent = piece == CELL_X ? CELL_O : CELL_X;

  if (!HasAnyPiece(board))
    return {BOARD_SIZE / 2, BOARD_SIZE / 2, piece};

  for (int row = 0; row < BOARD_SIZE; row++) {
    for (int col = 0; col < BOARD_SIZE; col++) {
      if (board[row][col] != CELL_EMPTY)
        continue;
      const long long score =
          EvaluatePositionMedium(row, col, piece, board) +
          EvaluatePositionMedium(row, col, opponent, board);
      if (score > bestScore) {
        bestScore = score;
        best = {row, col, piece};
      }
    }
  }
  return best;
}

void RunAIVsAIDemo(int gameCount) {
  if (gameCount < 1)
    gameCount = 1;

  int hardWins = 0;
  int greedyWins = 0;
  int draws = 0;
  std::printf("\n=== AI VS AI FAST SIMULATION ===\n");

  for (int gameIndex = 0; gameIndex < gameCount; gameIndex++) {
    DataGame game;
    ClearGame(&game);
    const int hardPiece = (gameIndex % 2 == 0) ? CELL_O : CELL_X;
    int moves = 0;

    while (moves < BOARD_SIZE * BOARD_SIZE) {
      GameMove move;
      if (game.turn == hardPiece) {
        SearchConfig config = {3, 50, 10};
        SearchResult search = FindHardMove(game.board, game.turn, config);
        move = {search.row, search.col, game.turn};
      } else {
        move = FindGreedyMove(game.board, game.turn);
      }

      GameMoveResult placed = ApplyMove(&game, move);
      if (!placed.accepted)
        break;
      moves++;

      GameWinInfo win =
          CheckWinOnBoard(game.board, move.row, move.col, 1);
      if (win.result != RESULT_ONGOING) {
        if (move.piece == hardPiece)
          hardWins++;
        else
          greedyWins++;
        break;
      }
      if (moves == BOARD_SIZE * BOARD_SIZE)
        draws++;
      game.turn = (game.turn == CELL_X) ? CELL_O : CELL_X;
    }

    std::printf("Game %d: %d moves, Hard=%c\n", gameIndex + 1, moves,
                hardPiece == CELL_X ? 'X' : 'O');
  }

  std::printf("Summary: Hard %d - Greedy %d - Draw %d\n", hardWins,
              greedyWins, draws);
}
