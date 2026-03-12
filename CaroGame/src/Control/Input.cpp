#include "../../include/Model.h"
#include "../../include/Model.h" // Phải include Model vì StartGame gọi ResetData, ExitGame gọi GarbageCollect
#include "../../include/View.h"  // Phải include View vì StartGame gọi DrawBoard

// --- Copy nội dung 6 hàm StartGame, ExitGame, MoveRight, MoveLeft, MoveDown, MoveUp từ source cũ vào đây ---

void StartGame() {
	system("cls");
	ResetData(); // Khởi tạo dữ liệu gốc
	DrawBoard(BOARD_SIZE); // Vẽ màn hình game
}

void ExitGame() {
	system("cls");
	GarbageCollect();
	//Có thể lưu game trước khi exit
}

void MoveRight() {
	if (_X < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].x) {
		_X += 4;
		GotoXY(_X, _Y);
	}
}

void MoveLeft() {
	if (_X > _A[0][0].x) {
		_X -= 4;
		GotoXY(_X, _Y);
	}
}

void MoveDown() {
	if (_Y < _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y) {
		_Y += 2;
		GotoXY(_X, _Y);
	}
}

void MoveUp() {
	if (_Y > _A[0][0].y) {
		_Y -= 2;
		GotoXY(_X, _Y);
	}
}
