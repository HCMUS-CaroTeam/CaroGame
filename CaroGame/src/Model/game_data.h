/**
 * @file game_data.h
 * @brief Định nghĩa cấu trúc dữ liệu trung tâm và các hằng số cốt lõi của trò chơi Caro.
 *
 * File này chứa:
 * - Các hằng số bàn cờ (kích thước, vị trí pixel, điều kiện thắng).
 * - Các enum định nghĩa trạng thái ô, kết quả, chế độ chơi, độ khó Bot.
 * - Struct DataGame — "Single Source of Truth" lưu toàn bộ trạng thái 1 ván game.
 * - Hàm current() — truy cập nhanh đến ván game hiện tại.
 *
 * @note Struct DataGame sử dụng `#pragma pack(push, 1)` để đảm bảo
 *       tuần tự hóa nhị phân (binary serialization) khi Save/Load game.
 */

#pragma once
#include "config.h"
#include <ctime>

/// @name Hằng số bàn cờ
/// @{
static constexpr int BOARD_SIZE = 15;          ///< Kích thước bàn cờ (15x15 ô).
static constexpr float CELL_SIZE = 38.0f;      ///< Kích thước mỗi ô tính bằng pixel.
static constexpr float BOARD_PIXEL_SIZE = BOARD_SIZE * CELL_SIZE; ///< Tổng kích thước bàn cờ (pixel).
static constexpr float BOARD_START_X = SCREEN_WIDTH * 0.5f - BOARD_PIXEL_SIZE * 0.5f; ///< Tọa độ X góc trên-trái bàn cờ (căn giữa màn hình).
static constexpr float BOARD_START_Y = 150.0f; ///< Tọa độ Y góc trên-trái bàn cờ (cố định).
static constexpr int WIN_LENGTH = 5;           ///< Số quân liên tiếp cần để thắng (đúng 5, không quá 5 — luật Overline).
static constexpr int MAX_WIN_LINE = BOARD_SIZE; ///< Kích thước tối đa mảng winLine.
/// @}

/**
 * @enum CellValue
 * @brief Giá trị của mỗi ô trên bàn cờ.
 */
enum CellValue {
    CELL_EMPTY = 0,  ///< Ô trống, chưa có quân.
    CELL_X = -1,     ///< Quân X (Người chơi 1 / Đi trước).
    CELL_O = 1       ///< Quân O (Người chơi 2 / Bot trong PvE).
};

/**
 * @enum GameResult
 * @brief Kết quả hiện tại của ván game.
 */
enum GameResult {
    RESULT_ONGOING = 0,  ///< Ván game đang diễn ra.
    RESULT_X_WINS = -1,  ///< Người chơi X thắng.
    RESULT_O_WINS = 1,   ///< Người chơi O thắng.
    RESULT_DRAW = 2      ///< Hòa (bàn cờ đầy mà không ai thắng).
};

/**
 * @enum GameMode
 * @brief Chế độ chơi chính.
 */
enum GameMode {
    MODE_PVP = 0,  ///< Player vs Player (2 người chơi).
    MODE_PVE = 1   ///< Player vs Environment (người chơi vs Bot AI).
};

/**
 * @enum BotDifficulty
 * @brief Mức độ khó của Bot AI (chỉ áp dụng khi gameMode == MODE_PVE).
 */
enum BotDifficulty {
	DIFFICULTY_NONE = -1,   ///< Giá trị khởi tạo, không phải mức độ khó thực sự.
    DIFFICULTY_EASY = 0,    ///< Bot Easy — Heuristic đơn giản + 25% random noise.
    DIFFICULTY_MEDIUM = 1,  ///< Bot Medium — Minimax depth-2 + neighbor filter.
    DIFFICULTY_HARD = 2     ///< Bot Hard — Minimax depth-5 + Alpha-Beta pruning.
};

/**
 * @enum PVPMode
 * @brief Chế độ phụ trong PvP (chỉ áp dụng khi gameMode == MODE_PVP).
 */
enum PVPMode {
	NONE = -1,       ///< Giá trị khởi tạo, không phải chế độ PVP thực sự.
	CLASSIC = 0,     ///< Classic — Đủ đúng 5 là thắng, không cần kiểm tra chặn 2 đầu. Vẫn áp dụng luật Overline.
	TOURNAMENT = 1   ///< Tournament — Đủ đúng 5 + không bị chặn 2 đầu mới thắng. Áp dụng luật Overline.
};

/**
 * @enum TimeLimit
 * @brief Giới hạn thời gian mỗi lượt đi (áp dụng cho Tournament và PvE).
 */
enum TimeLimit {
	TIME_LIMIT_NONE = 0, ///< Không giới hạn thời gian.
	TIME_LIMIT_5S = 1,   ///< Giới hạn 5 giây mỗi lượt.
	TIME_LIMIT_10S = 2,  ///< Giới hạn 10 giây mỗi lượt.
	TIME_LIMIT_15S = 3   ///< Giới hạn 15 giây mỗi lượt.
};

/**
 * @struct DataGame
 * @brief Cấu trúc dữ liệu lưu toàn bộ trạng thái của một ván game.
 *
 * Đây là "Single Source of Truth" — mọi module (Logic, AI, UI, Save/Load)
 * đều đọc/ghi thông qua struct này via hàm current().
 *
 * @warning Struct này dùng `#pragma pack(push, 1)` để đảm bảo layout nhị phân
 *          nhất quán khi serialize ra file. Thay đổi thứ tự hoặc thêm/xóa field
 *          sẽ phá vỡ tương thích với file save cũ.
 */
#pragma pack(push, 1)
struct DataGame {
	char nameGame[50];                ///< Tên ván game (hiển thị trong danh sách save).
	char namePlayer1[50];             ///< Tên người chơi 1 (X).
	int scorePlayer1;                 ///< Điểm tích lũy của người chơi 1 (qua nhiều ván).
	char namePlayer2[50];             ///< Tên người chơi 2 (O) hoặc tên Bot.
	int scorePlayer2;                 ///< Điểm tích lũy của người chơi 2.
	int board[BOARD_SIZE][BOARD_SIZE]; ///< Mảng trạng thái bàn cờ. @see CellValue
	int turn;                         ///< Lượt đi hiện tại. @see CellValue (CELL_X hoặc CELL_O).
	int result;                       ///< Kết quả ván game hiện tại. @see GameResult
	int gameMode;                     ///< Chế độ chơi. @see GameMode
	int botDifficulty;                ///< Mức độ khó Bot (chỉ dùng khi MODE_PVE). @see BotDifficulty
	int pvpMode;                      ///< Chế độ PVP (chỉ dùng khi MODE_PVP). @see PVPMode
	int timeLeft;                     ///< Cấu hình giới hạn thời gian mỗi lượt. @see TimeLimit
	time_t saveTime;                  ///< Timestamp lúc save game.
	int lastMoveRow;                  ///< Dòng (row) của nước đi cuối cùng (-1 nếu chưa có).
	int lastMoveCol;                  ///< Cột (col) của nước đi cuối cùng (-1 nếu chưa có).
	int winLine[MAX_WIN_LINE][2];     ///< Tọa độ [row, col] của các ô tạo thành đường thắng.
	int winLineCount;                 ///< Số ô thực tế trong winLine (luôn = WIN_LENGTH khi thắng).
};
#pragma pack(pop)

/**
 * @brief Lấy tham chiếu đến ván game hiện tại (global state).
 * @return Tham chiếu đến DataGame đang active.
 */
extern DataGame& GetCurrentGameData();

/**
 * @brief Shortcut truy cập nhanh đến ván game hiện tại.
 * @return Tham chiếu đến DataGame đang active.
 * @code
 *   current().board[r][c] = CELL_X;  // Đặt quân X
 *   int turn = current().turn;       // Lấy lượt đi
 * @endcode
 */
inline DataGame& current() {
	return GetCurrentGameData();
}
