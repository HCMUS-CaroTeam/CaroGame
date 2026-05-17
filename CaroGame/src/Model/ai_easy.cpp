#include "ai_easy.h"
#include "Model/game_data.h"
#include "raylib.h"

// ========================================================================
// HÀM HỖ TRỢ: Đếm số quân cờ liên tiếp của một phe theo một hướng nhất định
// ========================================================================
int CountConsecutive(int r, int c, int dr, int dc, int playerPiece) {
    int count = 0;
    // Quét tối đa 4 ô về một hướng
    for (int i = 1; i <= 4; i++) {
        int nr = r + dr * i;
        int nc = c + dc * i;

        // Nếu còn nằm trong bàn cờ và đúng quân của phe đó
        if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
            if (current().board[nr][nc] == playerPiece) {
                count++;
            }
            else {
                break; // Gặp ô trống hoặc quân địch thì dừng đếm
            }
        }
        else {
            break; // Đụng tường thì dừng
        }
    }
    return count;
}

// ========================================================================
// THUẬT TOÁN EASY: RULE-BASED (Tập Luật Cơ Bản)
// ========================================================================
void GetEasyMove(int& outRow, int& outCol) {
    // Xác định xem Bot đang cầm cờ gì, Người chơi cầm cờ gì
    int botPiece = current().turn;
    int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;

    // Các hướng cần quét: Ngang, Dọc, Chéo chính, Chéo phụ
    int dx[] = { 0, 1, 1, 1 };
    int dy[] = { 1, 0, 1, -1 };

    // --------------------------------------------------------------------
    // ƯU TIÊN 1: PHÒNG THỦ KHẨN CẤP (Chặn 3 hoặc 4 quân của người chơi)
    // --------------------------------------------------------------------
    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (current().board[r][c] == 0) { // Thử đặt giả định vào ô trống
                for (int dir = 0; dir < 4; dir++) {
                    // Đếm số quân địch ở 2 phía của ô trống này
                    int count = CountConsecutive(r, c, dx[dir], dy[dir], playerPiece) +
                        CountConsecutive(r, c, -dx[dir], -dy[dir], playerPiece);

                    // Nếu đánh vào đây chặn được 3 hoặc 4 quân địch -> ĐÁNH NGAY!
                    if (count >= 3) {
                        outRow = r;
                        outCol = c;
                        return; // Chốt hạ, không cần nghĩ thêm
                    }
                }
            }
        }
    }

    // --------------------------------------------------------------------
    // ƯU TIÊN 2: CẢM BIẾN KHÔNG GIAN (Đánh xung quanh các quân cờ đã có)
    // --------------------------------------------------------------------
    struct Point { int r, c; };
    Point potentials[BOARD_SIZE * BOARD_SIZE]; // Mảng lưu các ô trống tiềm năng
    int potCount = 0;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            if (current().board[r][c] == 0) {
                bool hasNeighbor = false;
                // Quét trong bán kính 2 ô xung quanh
                for (int dr = -2; dr <= 2 && !hasNeighbor; dr++) {
                    for (int dc = -2; dc <= 2 && !hasNeighbor; dc++) {
                        int nr = r + dr, nc = c + dc;
                        if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE) {
                            if (current().board[nr][nc] != 0) {
                                hasNeighbor = true; // Phát hiện có quân cờ ở gần
                            }
                        }
                    }
                }
                // Nếu có hàng xóm, đưa vào danh sách tiềm năng
                if (hasNeighbor) {
                    potentials[potCount] = { r, c };
                    potCount++;
                }
            }
        }
    }

    // Nếu tìm thấy các ô gần đó, bốc random 1 ô để đánh
    if (potCount > 0) {
        int idx = GetRandomValue(0, potCount - 1); // Hàm của Raylib
        outRow = potentials[idx].r;
        outCol = potentials[idx].c;
        return;
    }

    // --------------------------------------------------------------------
    // ƯU TIÊN 3: KHAI CUỘC (Bàn cờ trống trơn)
    // --------------------------------------------------------------------
    // Nếu chưa ai đánh gì (hoặc không tìm thấy ô nào), đánh thẳng vào giữa bàn cờ
    outRow = BOARD_SIZE / 2;
    outCol = BOARD_SIZE / 2;
}