#include "game_data.h"

static DataGame currentGame = {
	.nameGame = "",                   // Tên game mặc định, có thể được cập nhật khi người chơi lưu game
	.namePlayer1 = "",                // Tên người chơi 1 mặc định, có thể được cập nhật khi chơi game
	.scorePlayer1 = 0,                // Điểm số của người chơi 1 khởi tạo, sẽ được cập nhật khi chơi game
	.namePlayer2 = "",                // Tên người chơi 2 mặc định, có thể được cập nhật khi chơi game
	.scorePlayer2 = 0,                // Điểm số của người chơi 2 khởi tạo, sẽ được cập nhật khi chơi game
	.board = {},                      // Bàn cờ khởi tạo với tất cả ô trống, sẽ được cập nhật khi chơi game
	.turn = CELL_X,                   // Mặc định X đi trước, O đi sau, có thể được cập nhật khi bắt đầu game
	.result = RESULT_ONGOING,         // Kết quả mặc định là ongoing, có thể được cập nhật khi chơi game
	.gameMode = MODE_PVP,             // Chế độ chơi mặc định là PVP, có thể được cập nhật khi bắt đầu game
	.botDifficulty = DIFFICULTY_NONE, // Mức độ khó của Bot khởi tạo, chỉ áp dụng khi gameMode là MODE_PVE
	.pvpMode = NONE,                  // Chế độ PVP khởi tạo, chỉ áp dụng khi gameMode là MODE_PVP
	.timeLeft = TIME_LIMIT_NONE,     // Thời gian còn lại khởi tạo, chỉ áp dụng khi time limit được bật
	.saveTime = 0,                    // Thời gian lưu game khởi tạo, sẽ được cập nhật khi người chơi lưu game
	.lastMoveRow = -1,                // Dòng của nước đi cuối cùng khởi tạo, sẽ được cập nhật khi chơi game
	.lastMoveCol = -1,                // Cột của nước đi cuối cùng khởi tạo, sẽ được cập nhật khi chơi game
	.winLine = {{-1,-1}}               // Khởi tạo đường thắng với giá trị không hợp lệ
};

DataGame& GetCurrentGameData() {
	return currentGame;
}

