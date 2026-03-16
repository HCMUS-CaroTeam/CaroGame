#include "../../include/View.h"
#include "../../include/Model.h"
#include <raylib.h>

extern char _BOARD[BOARD_SIZE][BOARD_SIZE];
extern int _X, _Y;
extern bool _TURN;
extern short XScore, OScore, NumberOfRounds;

void DrawBoard(int size) {
	int cellSize = 30; // Kích thước ô
	int startX = 100;  // Lề trái
	int startY = 80;   // Lề trên

	for (int i = 0; i <= size; i++) {
		// Vẽ các đường kẻ lưới
		DrawLine(startX, startY + i * cellSize, startX + size * cellSize, startY + i * cellSize, BLACK);
		DrawLine(startX + i * cellSize, startY, startX + i * cellSize, startY + size * cellSize, BLACK);

		// Vẽ số tọa độ hàng và cột 
		if (i < size) {
			DrawText(TextFormat("%d", i), startX + i * cellSize + 10, startY - 20, 15, DARKGRAY); // Ngang
			DrawText(TextFormat("%d", i), startX - 25, startY + i * cellSize + 8, 15, DARKGRAY);  // Dọc
		}
	}
}

void DrawPieces() {
	int cellSize = 30;
	int startX = 100; 
	int startY = 80;

	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			// Kiểm tra giá trị trong mảng _BOARD
			if (_BOARD[i][j] == 'X' || _BOARD[i][j] == -1) {
				DrawText("X", startX + j * cellSize + 8, startY + i * cellSize + 4, 25, BLUE);
			}
			else if (_BOARD[i][j] == 'O' || _BOARD[i][j] == 1) {
				DrawText("O", startX + j * cellSize + 8, startY + i * cellSize + 4, 25, RED);
			}
		}
	}
}

// Qui ước: -1 = X thắng, 1 = O thắng, 0 = hòa, 2 = chưa hết game
int ProcessFinish(int pWhoWin)
{
	if (pWhoWin == -1) {
		XScore++; NumberOfRounds++;
	}
	else if (pWhoWin == 1) {
		OScore++; NumberOfRounds++;
	}
	else if (pWhoWin == 0) {
		NumberOfRounds++;
	}
	else {
		_TURN = !_TURN; // Game tiếp tục, đổi lượt
		return 2;
	}
	return pWhoWin;
}