#include "../../Shared/Common.h"
#include "Logic.h" // Chứa khai báo của các hàm bên dưới

// --- Định nghĩa các biến toàn cục từ Common.h ---
int _BOARD[BOARD_SIZE][BOARD_SIZE] = { {0} };
bool _TURN = true;           // true (X), false (O)
int _GAME_MODE = 1;          // 1: Tiêu chuẩn, 2: Hardcore
float _TIME_LEFT = TURN_TIME_LIMIT;     // Thời gian đếm ngược

int UpdateTimer() {
    // Nếu là Mode 1 (Tiêu chuẩn) thì bỏ qua, không thèm đếm
    if (_GAME_MODE == 1) return 0;

    // Nếu là Mode 2, bắt đầu trừ giờ
    _TIME_LEFT -= GetFrameTime();

    if (_TIME_LEFT <= 0.0f) {
        _TURN = !_TURN;                 // Phạt tước lượt
        _TIME_LEFT = TURN_TIME_LIMIT;   // Bơm lại 15s cho người kia
        return -99;                     // Tín hiệu báo cháy giờ để Control phát âm thanh
    }

    return 0; // Đang đếm bình thường
}

int CheckBoard(int pX, int pY) {
    if (pX < OFFSET_X || pY < OFFSET_Y) return 0; // Sửa lỗi tính toán sai khi click âm phía trên bên trái

    int col = (pX - OFFSET_X) / CELL_SIZE;
    int row = (pY - OFFSET_Y) / CELL_SIZE;

    // Rớt ngoài bàn cờ thì cút
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return 0;

    // Nếu ô trống thì cho đánh
    if (_BOARD[row][col] == 0) {
        _BOARD[row][col] = (_TURN == true) ? -1 : 1;

        // Đánh xong thì reset Timer (Nếu đang ở Mode 2)
        if (_GAME_MODE == 2) {
            _TIME_LEFT = TURN_TIME_LIMIT;
        }

        return _BOARD[row][col]; // Trả về con cờ vừa đánh
    }

    return 0; // Click trùng ô đã đánh
}

int TestBoard(int lastRow, int lastCol) {
    // 4 hướng: Ngang, Dọc, Chéo chính, Chéo phụ
    int dx[] = { 0, 1, 1, 1 };
    int dy[] = { 1, 0, 1, -1 };

    int player = _BOARD[lastRow][lastCol];
    if (player == 0) return 0;

    for (int dir = 0; dir < 4; dir++) {
        int count = 1;  // Tính luôn quân vừa đánh
        int blocks = 0; // Đếm số đầu bị chặn

        // Quét chiều TỚI
        for (int step = 1; step <= 5; step++) {
            int nr = lastRow + step * dx[dir];
            int nc = lastCol + step * dy[dir];

            if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
                blocks++; break; // Đụng tường = Bị chặn
            }
            if (_BOARD[nr][nc] == player) count++;
            else if (_BOARD[nr][nc] != 0) { blocks++; break; } // Đụng địch = Bị chặn
            else break; // Ô trống thì dừng
        }

        // Quét chiều LÙI
        for (int step = 1; step <= 5; step++) {
            int nr = lastRow - step * dx[dir];
            int nc = lastCol - step * dy[dir];

            if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
                blocks++; break;
            }
            if (_BOARD[nr][nc] == player) count++;
            else if (_BOARD[nr][nc] != 0) { blocks++; break; }
            else break;
        }

        // --- XÉT ĐIỀU KIỆN THẮNG THEO MODE ---
        if (count >= 5) {
            if (_GAME_MODE == 2) {
                // Hardcore: Phải không bị chặn 2 đầu mới cho thắng
                if (blocks < 2) return player;
            }
            else {
                // Tiêu chuẩn: Đủ 5 con là thắng
                return player;
            }
        }
    }

    return 0; // Chưa ai thắng
}