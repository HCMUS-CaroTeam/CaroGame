#include "ai_easy.h"
#include "Model/game_data.h"
#include "ai_evaluation.h"
#include "raylib.h"

// ========================================================================
// THUẬT TOÁN EASY: HEURISTIC ĐƠN GIẢN + RANDOM
// Dùng chung hệ thống đánh giá ai_evaluation với bảng điểm phẳng:
//   - Chỉ nhận ra WIN / OPEN_4 / BLOCKED_4 (mối nguy trước mắt)
//   - Phần còn lại gần như bằng 0 → bot đi ngẫu nhiên ở mid-game
// ========================================================================
void GetEasyMove(int& outRow, int& outCol) {
    int botPiece = current().turn;
    int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;

    long long maxScore = -1;
    struct Point { int r, c; };
    Point bestMoves[BOARD_SIZE * BOARD_SIZE];
    int bestCount = 0;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (current().board[r][c] != 0) continue;

            // Chỉ xét ô có hàng xóm trong bán kính 2
            bool hasNeighbor = false;
            for (int dr = -2; dr <= 2 && !hasNeighbor; dr++) {
                for (int dc = -2; dc <= 2 && !hasNeighbor; dc++) {
                    int nr = r + dr, nc = c + dc;
                    if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                        if (current().board[nr][nc] != 0)
                            hasNeighbor = true;
                    }
                }
            }
            if (!hasNeighbor) continue;

            // Dùng chung hệ thống đánh giá, bảng điểm Easy cực phẳng
            long long attackScore  = EvaluatePositionEasy(r, c, botPiece);
            long long defenseScore = EvaluatePositionEasy(r, c, playerPiece);
            long long totalScore   = attackScore + defenseScore;

            if (totalScore > maxScore) {
                maxScore = totalScore;
                bestCount = 0;
                bestMoves[bestCount++] = { r, c };
            }
            else if (totalScore == maxScore) {
                bestMoves[bestCount++] = { r, c };
            }
        }
    }

    // Lựa chọn nước đi
    if (bestCount > 0) {
        int idx = GetRandomValue(0, bestCount - 1);
        outRow = bestMoves[idx].r;
        outCol = bestMoves[idx].c;
    }
    else {
        // Khai cuộc: đánh giữa bàn cờ
        outRow = BOARD_SIZE / 2;
        outCol = BOARD_SIZE / 2;
    }
}