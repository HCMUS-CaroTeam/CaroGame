#include "ai_medium.h"
#include "Model/game_data.h"
#include "raylib.h"

// ========================================================================
// HÀM HỖ TRỢ CHO MEDIUM: CHẤM ĐIỂM THEO LUẬT CARO
// ========================================================================
long long CalculateLineScore(int count, int blocks) {
    // Nếu bị chặn 2 đầu mà chưa đủ 5 quân thì đường này vô dụng (luật Caro)
    if (blocks == 2 && count < 5) return 0;

    // Bảng điểm Heuristic: Đòn càng hiểm điểm càng cao
    switch (count) {
    case 5: return 100000000; // Đủ 5 quân: Thắng luôn (Vô giá)
    case 4: return (blocks == 0) ? 10000000 : 100000;  // 4 quân hở 2 đầu vs 1 đầu
    case 3: return (blocks == 0) ? 100000 : 10000;     // 3 quân hở 2 đầu vs 1 đầu
    case 2: return (blocks == 0) ? 10000 : 100;        // 2 quân hở 2 đầu vs 1 đầu
    case 1: return (blocks == 0) ? 10 : 1;             // 1 quân
    default: return 0;
    }
}

long long EvaluatePosition(int r, int c, int piece) {
    long long totalScore = 0;
    int dx[] = { 1, 0, 1, 1 };
    int dy[] = { 0, 1, 1, -1 };

    // Quét 4 hướng y hệt như lúc kiểm tra thắng thua
    for (int dir = 0; dir < 4; dir++) {
        int count = 1;
        int blocks = 0;

        // Quét chiều TỚI
        for (int step = 1; step <= 4; step++) {
            int nr = r + step * dx[dir];
            int nc = c + step * dy[dir];
            if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) { blocks++; break; }
            if (current().board[nr][nc] == piece) count++;
            else if (current().board[nr][nc] != 0) { blocks++; break; }
            else break;
        }

        // Quét chiều LÙI
        for (int step = 1; step <= 4; step++) {
            int nr = r - step * dx[dir];
            int nc = c - step * dy[dir];
            if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) { blocks++; break; }
            if (current().board[nr][nc] == piece) count++;
            else if (current().board[nr][nc] != 0) { blocks++; break; }
            else break;
        }

        // Cộng dồn điểm của hướng này vào tổng điểm của ô
        totalScore += CalculateLineScore(count, blocks);
    }
    return totalScore;
}

// ========================================================================
// THUẬT TOÁN MEDIUM: HEURISTIC EVALUATION (Đánh Giá Điểm Tĩnh)
// ========================================================================
void GetMediumMove(int& outRow, int& outCol) {
    int botPiece = current().turn;
    int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;

    long long maxScore = -1;

    // Tạo mảng hứng các ô có điểm CAO BẰNG NHAU để random (giúp Bot không bị đánh rập khuôn)
    struct Point { int r, c; };
    Point bestMoves[BOARD_SIZE * BOARD_SIZE];
    int bestCount = 0;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (current().board[r][c] == 0) { // Chỉ chấm điểm các ô trống

                // 1. Nếu Bot đánh vào đây thì lợi thế sức mạnh là bao nhiêu?
                long long attackScore = EvaluatePosition(r, c, botPiece);

                // 2. Nếu Người chơi đánh vào đây thì độ nguy hiểm là bao nhiêu?
                long long defenseScore = EvaluatePosition(r, c, playerPiece);

                // 3. Tổng điểm = Công + Thủ 
                // Mẹo: Nhân thủ với 1.5 để Bot ưu tiên tính "chắc cốp", thấy nguy hiểm là chặn ngay
                long long totalScore = attackScore + (defenseScore * 1.5);

                // Cập nhật kỷ lục mới
                if (totalScore > maxScore) {
                    maxScore = totalScore;
                    bestCount = 0; // Reset mảng kỷ lục
                    bestMoves[bestCount++] = { r, c };
                }
                // Nếu điểm bằng kỷ lục thì cho vào danh sách bốc thăm
                else if (totalScore == maxScore) {
                    bestMoves[bestCount++] = { r, c };
                }
            }
        }
    }

    // Bốc ngẫu nhiên 1 ô trong số các ô ngon nhất (Thường là ô điểm cao nhất duy nhất)
    if (bestCount > 0) {
        int idx = GetRandomValue(0, bestCount - 1);
        outRow = bestMoves[idx].r;
        outCol = bestMoves[idx].c;
    }
    else {
        // Nước đi đầu tiên nếu bàn cờ trống
        outRow = BOARD_SIZE / 2;
        outCol = BOARD_SIZE / 2;
    }
}