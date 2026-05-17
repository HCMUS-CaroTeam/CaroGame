#pragma once
#include "config.h"
#include <ctime>

static constexpr int BOARD_SIZE = 15;
static constexpr float CELL_SIZE = 48.0f;
static constexpr float BOARD_PIXEL_SIZE = BOARD_SIZE * CELL_SIZE; // Kích thước bàn cờ tính bằng pixel
static constexpr float BOARD_START_X = SCREEN_WIDTH * 0.5f - BOARD_PIXEL_SIZE * 0.5f; // Tính toán để bàn cờ nằm chính giữa
static constexpr float BOARD_START_Y = 120.0f; // Vị trí Y cố định để bàn cờ nằm ở phần trên màn hình
static constexpr int WIN_LENGTH = 5; // Số quân cần có liên tiếp để thắng

enum CellValue {
    CELL_EMPTY = 0,
    CELL_X = -1,
    CELL_O = 1
};

enum GameResult {
    RESULT_ONGOING = 0,
    RESULT_X_WINS = -1,
    RESULT_O_WINS = 1,
    RESULT_DRAW = 2
};

enum GameMode {
    MODE_PVP = 0,
    MODE_PVE = 1
};

enum BotDifficulty {
	DIFFICULTY_NONE = -1, // Chỉ dùng để khởi tạo, không phải là một mức độ khó thực sự
    DIFFICULTY_EASY = 0,
    DIFFICULTY_MEDIUM = 1,
    DIFFICULTY_HARD = 2
};

enum PVPMode {
	NONE = -1, // Chỉ dùng để khởi tạo, không phải là một chế độ PVP thực sự
	CLASSIC = 0,
	TOURNAMENT = 1
};

enum TimeLimit {
	TIME_LIMIT_NONE = 0,// Mode 1: Không giới hạn thời gian, bỏ qua phần đếm giờ
	TIME_LIMIT_5S = 1,
	TIME_LIMIT_10S = 2,
	TIME_LIMIT_15S = 3
};


#pragma pack(push, 1)
struct DataGame {
	char nameGame[50];				  // Tên game, dùng để hiển thị trong danh sách save
	char namePlayer1[50];			  // Tên người chơi 1, dùng để hiển thị trong danh sách save và khi load game
	int scorePlayer1;				  // Điểm số của người chơi 1, dùng để hiển thị trong danh sách save và khi load game
	char namePlayer2[50];			  // Tên người chơi 2, dùng để hiển thị trong danh sách save và khi load game
	int scorePlayer2;				  // Điểm số của người chơi 2, dùng để hiển thị trong danh sách save và khi load game
	int board[BOARD_SIZE][BOARD_SIZE];// Mảng lưu trạng thái bàn cờ, 0: trống, -1: O, 1: X
	int turn;                         // Lượt đi hiện tại, -1: O đi trước, 1: X đi sau
	int result;                       // Kết quả hiện tại của trò chơi
	int gameMode;                     // Chế độ chơi hiện tại
	int botDifficulty;                // Mức độ khó của Bot (chỉ áp dụng khi GAME_MODE là MODE_PVE)
	int pvpMode;                      // Chế độ chơi PVP (chỉ áp dụng khi GAME_MODE là MODE_PVP)
	int timeLeft;                     // Thời gian còn lại cho lượt đi hiện tại (chỉ áp dụng khi GAME_MODE là MODE_PVE)
	time_t saveTime;				  // Thời gian lưu game
	int lastMoveRow;				  // Dòng của nước đi cuối cùng, dùng để kiểm tra thắng thua và vẽ đường thắng
	int lastMoveCol;				  // Cột của nước đi cuối cùng, dùng để kiểm tra thắng thua và vẽ đường thắng
	int winLine[WIN_LENGTH][2];       // Mảng lưu tọa độ của các ô tạo thành đường thắng, chỉ có giá trị khi result là RESULT_X_WINS hoặc RESULT_O_WINS
};
#pragma pack(pop)

extern DataGame& GetCurrentGameData(); // Hàm trả về tham chiếu đến currentGame để các module ui_play, logic có thể truy cập và cập nhật trạng thái trò chơi hiện tại

inline DataGame& current() {
	return GetCurrentGameData();
}

