/**
 * @file Logic.h
 * @brief Khai báo các hàm xử lý logic cốt lõi của trò chơi Caro.
 *
 * Module này chịu trách nhiệm:
 * - Đặt quân cờ lên bàn cờ (CheckBoard).
 * - Kiểm tra điều kiện thắng — luật Overline (đúng 5, cấm quá 5)
 *   và luật chặn 2 đầu (tùy mode) (TestBoard).
 * - Quản lý timer đếm ngược mỗi lượt (UpdateTimer, GetTimeLeft).
 * - Reset/khởi tạo bàn cờ và ván game (ResetBoard, InitNewGame).
 * - Kiểm tra hòa (IsBoardFull).
 *
 * @note Luật áp dụng:
 *       - **Overline**: Áp dụng cho MỌI mode — chuỗi > 5 quân KHÔNG được tính thắng.
 *       - **Chặn 2 đầu**: Chỉ áp dụng cho Tournament và PvE — Classic bỏ qua.
 */

#ifndef LOGIC_H
#define LOGIC_H

#include "Model/game_data.h"

struct GameMove {
  int row;
  int col;
  int piece;
};

struct GameMoveResult {
  int accepted;
  int placedPiece;
};

struct GameWinInfo {
  int result;
  int lineCount;
  int line[MAX_WIN_LINE][2];
};

int IsValidMove(const int board[][BOARD_SIZE], int row, int col);
GameMoveResult ApplyMove(DataGame *game, GameMove move);
GameWinInfo CheckWinOnBoard(const int board[][BOARD_SIZE], int lastRow,
                            int lastCol, int applyBlockRule);
void ResetTurnTimer();

/**
 * @brief Cập nhật bộ đếm thời gian mỗi frame.
 *
 * Chỉ hoạt động khi mode có giới hạn thời gian (PVP Tournament hoặc PVE).
 * Khi hết giờ, tự động đổi lượt và reset bộ đếm.
 *
 * @return -99 nếu vừa hết giờ (để UI phát âm thanh cảnh báo), 0 nếu bình thường.
 */
int UpdateTimer();

/**
 * @brief Đặt quân cờ lên bàn cờ dựa trên tọa độ click chuột.
 *
 * Chuyển đổi tọa độ pixel → tọa độ mảng [row][col], kiểm tra tính hợp lệ
 * (trong phạm vi bàn cờ, ô chưa có quân), và cập nhật board.
 * Tự động reset timer sau khi đặt quân thành công (nếu mode có timer).
 *
 * @param pX Tọa độ pixel X của chuột.
 * @param pY Tọa độ pixel Y của chuột.
 * @return Giá trị quân cờ vừa đặt (CELL_X hoặc CELL_O), hoặc 0 nếu không hợp lệ.
 */
int CheckBoard(int pX, int pY);

/**
 * @brief Kiểm tra điều kiện thắng sau nước đi cuối cùng.
 *
 * Quét 4 hướng (ngang, dọc, chéo chính, chéo phụ) từ vị trí nước đi cuối.
 *
 * **Luật Overline (mọi mode):** Chỉ thắng khi chuỗi liên tiếp **đúng bằng** WIN_LENGTH (5).
 * Chuỗi > 5 quân (Overline) KHÔNG được tính thắng.
 *
 * **Luật chặn 2 đầu (tùy mode):**
 * - Tournament / PvE: chuỗi 5 bị chặn cả 2 đầu → KHÔNG thắng.
 * - Classic: bỏ qua luật này — đủ đúng 5 là thắng.
 *
 * Khi phát hiện thắng, tự động:
 * - Lưu đường thắng vào current().winLine (đúng 5 ô).
 * - Tăng điểm cho người chơi thắng.
 *
 * @param lastRow Dòng (row) của nước đi cuối cùng.
 * @param lastCol Cột (col) của nước đi cuối cùng.
 * @return RESULT_X_WINS (-1), RESULT_O_WINS (1), hoặc RESULT_ONGOING (0).
 *
 * @see GameResult
 */
int TestBoard(int lastRow, int lastCol);

/**
 * @brief Reset bàn cờ về trạng thái trống, giữ nguyên thông tin ván game.
 *
 * Xóa toàn bộ quân cờ, đặt turn = CELL_X, result = RESULT_ONGOING,
 * reset timer, lastMove, và winLine. Không reset tên/điểm người chơi.
 */
extern void ResetBoard();

/**
 * @brief Khởi tạo ván game hoàn toàn mới.
 *
 * Gọi ResetBoard() và thêm reset tên người chơi, điểm số, chế độ chơi,
 * độ khó bot, và tất cả cấu hình về giá trị mặc định.
 */
extern void InitNewGame();

/**
 * @brief Kiểm tra bàn cờ đã đầy hay chưa (dùng để xét kết quả hòa).
 * @return true nếu không còn ô trống nào, false nếu vẫn còn ô trống.
 */
extern bool IsBoardFull();

/**
 * @brief Lấy thời gian còn lại của lượt đi hiện tại.
 *
 * Dùng cho UI hiển thị countdown timer.
 *
 * @return Thời gian còn lại (giây), hoặc -1.0f nếu mode không có giới hạn thời gian.
 */
extern float GetTimeLeft();

#endif // LOGIC_H
