#include "ai_medium.h"
#include "Model/game_data.h"
#include "raylib.h"
#include "ai_evaluation.h"

// ========================================================================
// THUẬT TOÁN MEDIUM: HEURISTIC EVALUATION (Đánh Giá Điểm Tĩnh)
// Phiên bản mới: Dùng "Simulate & Compare" để phòng thủ thông minh
// + Attack Bias để bot chủ động tấn công
// ========================================================================
void GetMediumMove(int& outRow, int& outCol) {
    int botPiece = current().turn;
    int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;

    long long maxScore = -1;
    struct Point { int r, c; };
    Point bestMoves[BOARD_SIZE * BOARD_SIZE];
    int bestCount = 0;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (current().board[r][c] == 0) {
                // 1. Tính điểm CÔNG - Đặt vào đây Bot được lợi gì?
                long long attackScore = EvaluatePositionMedium(r, c, botPiece);
                // 2. Nếu Người chơi đánh vào đây thì độ nguy hiểm là bao nhiêu?
                long long defenseScore = EvaluatePositionMedium(r, c, playerPiece);

                // 3. Tổng điểm = Công + Thủ 
                // Mẹo: Nhân thủ với 1.5 để Bot ưu tiên tính "chắc cốp", thấy nguy hiểm là chặn ngay
                long long totalScore = attackScore + (defenseScore * 1.5);

                //// Ưu tiên Tuyệt đối #1: Đánh vào đây là THẮNG LUÔN -> Đánh ngay.
                //if (attackScore >= SCORE_WIN) {
                //    outRow = r; outCol = c;
                //    return; 
                //}

                //// 2. Tính điểm THỦ - Nếu không chạm vào, đối thủ đặt vào đây nguy hiểm cỡ nào?
                //long long defenseScore = EvaluatePosition(r, c, playerPiece);

                //long long totalScore = 0;

                //// Ưu tiên Tuyệt đối #2: Đối thủ Mở 4 hoặc Sắp Thắng -> BẮT BUỘC PHẢI CHẶN.
                //// Cho điểm cực lớn để bot ưu tiên ô này hơn bất kỳ ô tấn công lặt vặt nào.
                //if (defenseScore >= SCORE_OPEN4) {
                //    totalScore = SCORE_OPEN4 * 10; 
                //}
                //// Nếu mình không cận kề cái chết, thì tính điểm cộng dồn bình thường:
                //else {
                //    // Cân bằng lại: Ở mức Medium, tính "cẩn thận" vẫn cần đặt lên cao hơn 1 xíu.
                //    // Việc cộng dồn cả hai sẽ tự động chọn ô "vừa công vừa thủ" tốt nhất.
                //    totalScore = attackScore + (defenseScore * 1.5);
                //}

                // Cập nhật kỷ lục
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
    }

    // Lựa chọn nước đi
    if (bestCount > 0) {
        int idx = GetRandomValue(0, bestCount - 1);
        outRow = bestMoves[idx].r;
        outCol = bestMoves[idx].c;
    }
    else {
        outRow = BOARD_SIZE / 2;
        outCol = BOARD_SIZE / 2;
    }
}