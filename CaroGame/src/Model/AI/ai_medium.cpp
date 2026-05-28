#include "ai_medium.h"
#include "Model/game_data.h"
#include "ai_evaluation.h"
#include "raylib.h"
#include <algorithm>
#include <cstring>

// ========================================================================
// THUẬT TOÁN MEDIUM: MINIMAX DEPTH-2
// Bot "nghĩ trước" 1 nước của đối thủ, mạnh hơn hẳn Greedy nhưng
// vẫn yếu hơn Hard (depth-5 + Alpha-Beta + Move Ordering nâng cao).
// ========================================================================

static constexpr int MEDIUM_MAX_DEPTH = 2;
static constexpr int MEDIUM_BEAM_WIDTH = 12;
static const long long MEDIUM_INF = 10000000000000000LL;

typedef int ScratchBoard[BOARD_SIZE][BOARD_SIZE];

struct MediumMove {
  int r, c;
  long long score;
};

// ========================================================================
// SAO CHÉP BÀN CỜ
// ========================================================================
static void CopyBoard(const int src[BOARD_SIZE][BOARD_SIZE], ScratchBoard dst) {
  memcpy(dst, src, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
}

// ========================================================================
// TÌM CÁC NƯỚC ĐI TIỀM NĂNG (Neighbor filter + Move Ordering)
// ========================================================================
static int GenerateMovesMedium(MediumMove moves[], int botPiece,
                                int playerPiece, const ScratchBoard scratch) {
  int count = 0;
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      if (scratch[r][c] != 0)
        continue;

      // Neighbor filter: bán kính 2
      bool hasNeighbor = false;
      for (int dr = -2; dr <= 2 && !hasNeighbor; dr++) {
        for (int dc = -2; dc <= 2 && !hasNeighbor; dc++) {
          int nr = r + dr, nc = c + dc;
          if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE &&
              scratch[nr][nc] != 0) {
            hasNeighbor = true;
          }
        }
      }
      if (!hasNeighbor)
        continue;

      // Chấm điểm tức thời để ưu tiên (Move Ordering)
      long long atk = EvaluatePositionMedium(r, c, botPiece, scratch);
      long long def = EvaluatePositionMedium(r, c, playerPiece, scratch);
      moves[count++] = {r, c, atk + def};
    }
  }

  // Sort giảm dần để nước có điểm cao nhất xét trước
  std::sort(moves, moves + count,
            [](const MediumMove &a, const MediumMove &b) {
              return a.score > b.score;
            });

  // Giới hạn beam width
  return std::min(count, MEDIUM_BEAM_WIDTH);
}

// ========================================================================
// ĐÁNH GIÁ TOÀN CỤC BÀN CỜ (Dùng bảng điểm Medium)
// ========================================================================
static long long EvaluateBoardMedium(int botPiece, int playerPiece,
                                      const ScratchBoard scratch) {
  long long totalScore = 0;
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      if (scratch[r][c] == botPiece) {
        long long score = EvaluatePositionMedium(r, c, botPiece, scratch);
        if (score >= MEDIUM_SCORES[WIN])
          return MEDIUM_INF / 2;
        totalScore += score;
      } else if (scratch[r][c] == playerPiece) {
        long long score = EvaluatePositionMedium(r, c, playerPiece, scratch);
        if (score >= MEDIUM_SCORES[WIN])
          return -MEDIUM_INF / 2;
        totalScore -= score;
      }
    }
  }
  return totalScore;
}

// ========================================================================
// MINIMAX DEPTH-2 (Đơn giản, không cần Alpha-Beta vì depth quá nông)
// ========================================================================
static long long MinimaxMedium(int depth, bool isMaximizing, int botPiece,
                                int playerPiece, ScratchBoard scratch) {
  if (depth == 0)
    return EvaluateBoardMedium(botPiece, playerPiece, scratch);

  MediumMove moves[BOARD_SIZE * BOARD_SIZE];
  int moveCount =
      GenerateMovesMedium(moves, botPiece, playerPiece, scratch);

  if (moveCount == 0)
    return EvaluateBoardMedium(botPiece, playerPiece, scratch);

  if (isMaximizing) {
    long long maxEval = -MEDIUM_INF;
    for (int i = 0; i < moveCount; i++) {
      int r = moves[i].r;
      int c = moves[i].c;

      scratch[r][c] = botPiece;

      long long eval;
      // Dừng sớm nếu thắng luôn
      if (EvaluatePositionMedium(r, c, botPiece, scratch) >=
          MEDIUM_SCORES[WIN]) {
        eval = MEDIUM_INF / 2 + depth;
      } else {
        eval = MinimaxMedium(depth - 1, false, botPiece, playerPiece, scratch);
      }

      scratch[r][c] = 0;

      if (eval > maxEval)
        maxEval = eval;
    }
    return maxEval;
  } else {
    long long minEval = MEDIUM_INF;
    for (int i = 0; i < moveCount; i++) {
      int r = moves[i].r;
      int c = moves[i].c;

      scratch[r][c] = playerPiece;

      long long eval;
      if (EvaluatePositionMedium(r, c, playerPiece, scratch) >=
          MEDIUM_SCORES[WIN]) {
        eval = -MEDIUM_INF / 2 - depth;
      } else {
        eval = MinimaxMedium(depth - 1, true, botPiece, playerPiece, scratch);
      }

      scratch[r][c] = 0;

      if (eval < minEval)
        minEval = eval;
    }
    return minEval;
  }
}

// ========================================================================
// HÀM CHÍNH: TÌM NƯỚC ĐI CHO BOT MEDIUM
// ========================================================================
void GetMediumMove(int &outRow, int &outCol) {
  int botPiece = current().turn;
  int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;

  // Tạo scratch board
  ScratchBoard scratch;
  CopyBoard(current().board, scratch);

  MediumMove moves[BOARD_SIZE * BOARD_SIZE];
  int moveCount =
      GenerateMovesMedium(moves, botPiece, playerPiece, scratch);

  if (moveCount == 0) {
    outRow = BOARD_SIZE / 2;
    outCol = BOARD_SIZE / 2;
    return;
  }

  // ƯU TIÊN #1: Kiểm tra thắng ngay 1 nước
  for (int i = 0; i < moveCount; i++) {
    int r = moves[i].r, c = moves[i].c;
    scratch[r][c] = botPiece;
    if (EvaluatePositionMedium(r, c, botPiece, scratch) >=
        MEDIUM_SCORES[WIN]) {
      outRow = r;
      outCol = c;
      return;
    }
    scratch[r][c] = 0;
  }

  // ƯU TIÊN #2: Chặn đối thủ thắng ngay
  for (int i = 0; i < moveCount; i++) {
    int r = moves[i].r, c = moves[i].c;
    scratch[r][c] = playerPiece;
    if (EvaluatePositionMedium(r, c, playerPiece, scratch) >=
        MEDIUM_SCORES[WIN]) {
      outRow = r;
      outCol = c;
      scratch[r][c] = 0;
      return;
    }
    scratch[r][c] = 0;
  }

  // ƯU TIÊN #3: Minimax depth-2
  long long bestScore = -MEDIUM_INF;
  int bestRow = -1, bestCol = -1;

  for (int i = 0; i < moveCount; i++) {
    int r = moves[i].r, c = moves[i].c;
    scratch[r][c] = botPiece;

    long long score = MinimaxMedium(MEDIUM_MAX_DEPTH - 1, false, botPiece,
                                     playerPiece, scratch);
    scratch[r][c] = 0;

    if (score > bestScore) {
      bestScore = score;
      bestRow = r;
      bestCol = c;
    }
  }

  outRow = bestRow;
  outCol = bestCol;
}