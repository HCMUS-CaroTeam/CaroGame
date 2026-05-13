#include "ai_hard.h"
#include "Model/game_data.h"
#include <algorithm> // Để xài hàm std::max, std::min

#define MAX_DEPTH 3 // CẢNH BÁO: Không nên để quá 3, máy tính sẽ tính rất lâu!
#define INF 1000000000 // Đại diện cho vô cực

// Tái sử dụng hàm tính điểm từ Medium (Cần khai báo lại hoặc #include nếu dùng chung)
extern long long CalculateLineScore(int count, int blocks);
extern long long EvaluatePosition(int r, int c, int piece);

struct Move { int r, c; };

// ========================================================================
// HÀM HỖ TRỢ 1: TÌM CÁC NƯỚC ĐI TIỀM NĂNG (Khoanh vùng tìm kiếm)
// ========================================================================
int GeneratePotentialMoves(Move moves[]) {
    int count = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (current().board[r][c] == 0) {
                bool hasNeighbor = false;
                // Chỉ lấy những ô trống có quân cờ ở gần (Bán kính 2 ô)
                for (int dr = -2; dr <= 2 && !hasNeighbor; dr++) {
                    for (int dc = -2; dc <= 2 && !hasNeighbor; dc++) {
                        int nr = r + dr, nc = c + dc;
                        if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE && current().board[nr][nc] != 0) {
                            hasNeighbor = true;
                        }
                    }
                }
                if (hasNeighbor) {
                    moves[count++] = { r, c };
                }
            }
        }
    }
    return count;
}

// ========================================================================
// HÀM HỖ TRỢ 2: ĐÁNH GIÁ TOÀN CỤC BÀN CỜ
// ========================================================================
long long EvaluateBoard(int botPiece, int playerPiece) {
    long long totalScore = 0;
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (current().board[r][c] == botPiece) {
                totalScore += EvaluatePosition(r, c, botPiece); // Điểm cộng cho Bot
            }
            else if (current().board[r][c] == playerPiece) {
                totalScore -= EvaluatePosition(r, c, playerPiece) * 1.5; // Điểm trừ nặng nếu người chơi có thế cờ tốt
            }
        }
    }
    return totalScore;
}

// ========================================================================
// THUẬT TOÁN MINIMAX KẾT HỢP ALPHA-BETA PRUNING
// ========================================================================
long long Minimax(int depth, long long alpha, long long beta, bool isMaximizing, int botPiece, int playerPiece) {
    // Điều kiện dừng: Chạm đáy độ sâu
    if (depth == 0) {
        return EvaluateBoard(botPiece, playerPiece);
    }

    Move potentialMoves[BOARD_SIZE * BOARD_SIZE];
    int moveCount = GeneratePotentialMoves(potentialMoves);

    // Nếu hết nước đi hoặc bàn cờ đầy
    if (moveCount == 0) return EvaluateBoard(botPiece, playerPiece);

    if (isMaximizing) { // Lượt của BOT (Tìm điểm cao nhất)
        long long maxEval = -INF;
        for (int i = 0; i < moveCount; i++) {
            int r = potentialMoves[i].r;
            int c = potentialMoves[i].c;

            current().board[r][c] = botPiece; // Đánh thử
            long long eval = Minimax(depth - 1, alpha, beta, false, botPiece, playerPiece);
            current().board[r][c] = 0; // Trả lại bàn cờ cũ (Backtracking)

            maxEval = std::max(maxEval, eval);
            alpha = std::max(alpha, eval);
            if (beta <= alpha) break; // Cắt tỉa nhánh Beta (Nhánh vô dụng)
        }
        return maxEval;
    }
    else { // Lượt của NGƯỜI CHƠI (Tưởng tượng người chơi chọn điểm thấp nhất cho Bot)
        long long minEval = INF;
        for (int i = 0; i < moveCount; i++) {
            int r = potentialMoves[i].r;
            int c = potentialMoves[i].c;

            current().board[r][c] = playerPiece; // Người đánh thử
            long long eval = Minimax(depth - 1, alpha, beta, true, botPiece, playerPiece);
            current().board[r][c] = 0; // Backtracking

            minEval = std::min(minEval, eval);
            beta = std::min(beta, eval);
            if (beta <= alpha) break; // Cắt tỉa nhánh Alpha
        }
        return minEval;
    }
}

// ========================================================================
// HÀM CHÍNH CHO TRÙM CUỐI
// ========================================================================
void GetHardMove(int& outRow, int& outCol) {
    int botPiece = current().turn;
    int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;

    Move potentialMoves[BOARD_SIZE * BOARD_SIZE];
    int moveCount = GeneratePotentialMoves(potentialMoves);

    if (moveCount == 0) {
        outRow = BOARD_SIZE / 2;
        outCol = BOARD_SIZE / 2;
        return;
    }

    long long bestScore = -INF;
    int bestRow = -1, bestCol = -1;

    // Duyệt qua các nước đi tiềm năng lớp ngoài cùng
    for (int i = 0; i < moveCount; i++) {
        int r = potentialMoves[i].r;
        int c = potentialMoves[i].c;

        current().board[r][c] = botPiece; // Đánh thử bước đầu tiên
        // Gọi Minimax cho các bước tiếp theo
        long long score = Minimax(MAX_DEPTH - 1, -INF, INF, false, botPiece, playerPiece);
        current().board[r][c] = 0; // Backtracking

        // Tìm nước đi sinh ra điểm số Minimax cao nhất
        if (score > bestScore) {
            bestScore = score;
            bestRow = r;
            bestCol = c;
        }
    }

    outRow = bestRow;
    outCol = bestCol;
}