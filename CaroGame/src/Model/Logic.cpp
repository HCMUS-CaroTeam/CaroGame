#include "Model/game_data.h"
#include "Logic.h" // Chứa khai báo của các hàm bên dưới
#include "../raylib/include/raylib.h"

static float TIME_LIMIT = 0.0f; // Thời gian tối đa cho mỗi lượt đi, dùng để reset khi hết giờ
static float TIME_LEFT = 0.0f; // Biến toàn cục để theo dõi thời gian còn lại cho lượt đi hiện tại
static bool timerInitialized = false; // Biến toàn cục để theo dõi trạng thái khởi tạo timer

int UpdateTimer() {
	// Chỉ đếm giờ khi đang ở chế độ PVP TOURNAMENT hoặc MODE_PVE, còn lại thì bỏ qua phần đếm giờ
	// Nếu không làm Tournament thì comment dòng này đi
	if (current().gameMode == MODE_PVP && current().pvpMode != TOURNAMENT) 

        return 0; // Mode PVP khác mode tournament không có giới hạn thời gian
	// if (current().timeLeft == TIME_LIMIT_NONE)
    //       return 0; // Mode PVP CLASSIC không giới hạn thời gian

	if (!timerInitialized) {
		timerInitialized = true; // Đánh dấu đã khởi tạo timer
        switch (current().timeLeft)
        {
        case TIME_LIMIT_5S:
            TIME_LIMIT = 5.0f;
            break;
        case TIME_LIMIT_10S:
            TIME_LIMIT = 10.0f;
            break;
        case TIME_LIMIT_15S:
            TIME_LIMIT = 15.0f;
            break;
        default:
            break;
        }
        TIME_LEFT = TIME_LIMIT; // Khởi tạo thời gian còn lại bằng thời gian giới hạn
    }
	
    TIME_LEFT -= GetFrameTime();

    if (TIME_LEFT <= 0.0f) {
        current().turn = -current().turn;   // Phạt tước lượt
        TIME_LEFT = TIME_LIMIT;             // Bơm lại 15s cho người kia
        return -99;                         // Tín hiệu báo cháy giờ để Control phát âm thanh
    }

    return 0; // Đang đếm bình thường
}

int CheckBoard(int pX, int pY) {
    if (pX < BOARD_START_X || pY < BOARD_START_Y) return 0; // Sửa lỗi tính toán sai khi click âm phía trên bên trái

    int col = (pX - BOARD_START_X) / CELL_SIZE;
    int row = (pY - BOARD_START_Y) / CELL_SIZE;

    // Rớt ngoài bàn cờ thì cút
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) return 0;

    // Nếu ô trống thì cho đánh
    if (current().board[row][col] == 0) {
        current().board[row][col] = (current().turn == CELL_X) ? CELL_X : CELL_O;

        // Đánh xong thì reset Timer 
        // Chỉ reset timer khi đang ở chế độ có giới hạn thời gian
		if (current().gameMode == MODE_PVE || current().pvpMode == TOURNAMENT) { 
            TIME_LEFT = TIME_LIMIT;
        }

        return current().board[row][col]; // Trả về con cờ vừa đánh
    }

    return 0; // Click trùng ô đã đánh
}

int TestBoard(int lastRow, int lastCol) {
    // 4 hướng: Ngang, Dọc, Chéo chính, Chéo phụ
    int dx[] = { 0, 1, 1, 1 };
    int dy[] = { 1, 0, 1, -1 };

    int player = current().board[lastRow][lastCol];
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
            if (current().board[nr][nc] == player) count++;
            else if (current().board[nr][nc] != 0) { blocks++; break; } // Đụng địch = Bị chặn
            else break; // Ô trống thì dừng
        }

        // Quét chiều LÙI
        for (int step = 1; step <= 5; step++) {
            int nr = lastRow - step * dx[dir];
            int nc = lastCol - step * dy[dir];

            if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
                blocks++; break;
            }
            if (current().board[nr][nc] == player) count++;
            else if (current().board[nr][nc] != 0) { blocks++; break; }
            else break;
        }

        // --- XÉT ĐIỀU KIỆN THẮNG THEO MODE ---
        if (count >= 5) {
            bool isWin = false;
            if (current().pvpMode == TOURNAMENT || current().gameMode == MODE_PVE) {
                if (blocks < 2) isWin = true;
            }
            else {
                isWin = true;
            }

            if (isWin) {
				// Nếu nthắng thì lưu lại đường thắng vào current().winLine để UI có thể vẽ đường thắng
                
                // Tìm phạm vi của chuỗi quân cờ liên tiếp (để tránh lấy nhầm quân không liền kề hoặc vượt quá mảng)
                int startStep = 0;
                while (true) {
                    int nr = lastRow + (startStep - 1) * dx[dir];
                    int nc = lastCol + (startStep - 1) * dy[dir];
                    if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) break;
                    if (current().board[nr][nc] == player) startStep--;
                    else break;
                }

                int endStep = 0;
                while (true) {
                    int nr = lastRow + (endStep + 1) * dx[dir];
                    int nc = lastCol + (endStep + 1) * dy[dir];
                    if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) break;
                    if (current().board[nr][nc] == player) endStep++;
                    else break;
                }

                int idx = 0;
                for (int step = startStep; step <= endStep; step++) {
                    if (idx >= WIN_LENGTH) break;
                    int nr = lastRow + step * dx[dir];
                    int nc = lastCol + step * dy[dir];
                    current().winLine[idx][0] = nr;
                    current().winLine[idx][1] = nc;
                    idx++;
                }
                if (player == CELL_X)
                {
                    current().scorePlayer1++;
                }
				else if (player == CELL_O)
                {
                    current().scorePlayer2++;
                }
                // Cập nhật kết quả vào current() để Control có thể xử lý
				return (player == CELL_X) ? RESULT_X_WINS : RESULT_O_WINS;
            }
        }
    }
	return RESULT_ONGOING; // Chưa có ai thắng, tiếp tục chơi
}

void ResetBoard() {
    for (int r = 0; r < BOARD_SIZE; r++)
        for (int c = 0; c < BOARD_SIZE; c++)
            current().board[r][c] = 0;
    current().turn = CELL_X; // Mặc định X đi trước
	current().result = RESULT_ONGOING; // Reset kết quả về ongoing khi reset bàn cờ
    timerInitialized = false; // Reset trạng thái khởi tạo timer khi reset bàn cờ
	current().lastMoveRow = -1; // Reset vị trí nước đi cuối cùng về giá trị không hợp lệ
	current().lastMoveCol = -1; // Reset vị trí nước đi cuối cùng về giá trị không hợp lệ
	current().winLine[0][0] = -1; // Reset đường thắng về giá trị không hợp lệ
}

void InitNewGame() {
    ResetBoard();
	current().nameGame[0] = '\0'; // Reset tên game
	current().namePlayer1[0] = '\0'; // Reset tên người chơi 1
	current().scorePlayer1 = 0; // Reset điểm số người chơi 1
	current().namePlayer2[0] = '\0'; // Reset tên người chơi 2
	current().scorePlayer2 = 0; // Reset điểm số người chơi 2
	current().gameMode = MODE_PVP; // Mặc định chế độ chơi là PVP
	current().botDifficulty = DIFFICULTY_NONE; // Mặc định không có Bot
	current().pvpMode = NONE; // Mặc định không có chế độ PVP đặc biệt
	current().timeLeft = TIME_LIMIT_NONE; // Mặc định không giới hạn thời gian
    TIME_LIMIT = 0.0f;
	TIME_LEFT = 0.0f;
    // Có thể thêm các thiết lập khác khi bắt đầu game mới nếu cần
}

bool IsBoardFull() {
    for (int r = 0; r < BOARD_SIZE; r++)
        for (int c = 0; c < BOARD_SIZE; c++)
            if (current().board[r][c] == 0) return false;
    return true;
}