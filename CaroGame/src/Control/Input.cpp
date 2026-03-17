#include "../../include/Model.h"   // Phải include Model vì StartGame gọi ResetData, ExitGame gọi GarbageCollect
#include "../../include/View.h"	   // Phải include View vì StartGame gọi DrawBoard
#include "../../include/Control.h" // Phải include Common vì các hàm sử dụng _X, _Y

// --- Copy nội dung 6 hàm StartGame, ExitGame, MoveRight, MoveLeft, MoveDown, MoveUp từ source cũ vào đây ---

void StartGame(bool resetAll)
{
	ResetData(resetAll);		   // Khởi tạo dữ liệu gốc
	DrawBoard(BOARD_SIZE); // Vẽ màn hình game
}

void ExitGame()
{
	// Có thể lưu game trước khi exit
}

void MoveRight() {
	if (_COL < BOARD_SIZE - 1) {
		_COL++;
		_X += 30; // 30 là kích thước 1 ô cờ 
	}
}

void MoveLeft()
{
	if (_COL > 0)
	{
		_COL--;
		_X -= 30;
	}
}

void MoveDown()
{
	if (_ROW < BOARD_SIZE - 1)
	{
		_ROW++;
		_Y += 30;
	}
}

void MoveUp()
{
	if (_ROW > 0)
	{
		_ROW--;
		_Y -= 30;
	}
}
