#include "Model.h"

// ĐỊNH NGHĨA BIẾN TOÀN CỤC TẠI ĐÂY (Chỉ 1 lần duy nhất)
_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN;
int _COMMAND;
int _X, _Y;

// --- Copy nội dung 4 hàm ResetData, GabageCollect, TestBoard, CheckBoard từ source cũ của bạn vào đây ---
void ResetData() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			_A[i][j].x = 4 * j + LEFT + 2; // Trùng với hoành độ màn hình bàn cờ
			_A[i][j].y = 2 * i + TOP + 1; // Trùng với tung độ màn hình bàn cờ
			_A[i][j].c = 0; // 0 nghĩa là chưa ai đánh dấu, nếu đánh dấu phải theo quy
			//định như sau: -1 là lượt true đánh, 1 là lượt false đánh
		}
	}
	_TURN = true; _COMMAND = -1; // Gán lượt và phím mặc định
	_X = _A[0][0].x; _Y = _A[0][0].y; // Thiết lập lại tọa độ hiện hành ban đầu
}

void GabageCollect() {
	// Dọn dẹp tài nguyên nếu có khai báo con trỏ
}

int TestBoard() {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (_A[i][j].c != 0) {
				int current = _A[i][j].c;
				// Kiểm tra hàng ngang
				if (j <= BOARD_SIZE - 5 && _A[i][j + 1].c == current && _A[i][j + 2].c == current && _A[i][j + 3].c == current && _A[i][j + 4].c == current) return current;
				// Kiểm tra hàng dọc
				if (i <= BOARD_SIZE - 5 && _A[i + 1][j].c == current && _A[i + 2][j].c == current && _A[i + 3][j].c == current && _A[i + 4][j].c == current) return current;
				// Kiểm tra chéo xuôi
				if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5 && _A[i + 1][j + 1].c == current && _A[i + 2][j + 2].c == current && _A[i + 3][j + 3].c == current && _A[i + 4][j + 4].c == current) return current;
				// Kiểm tra chéo ngược
				if (i >= 4 && j <= BOARD_SIZE - 5 && _A[i - 1][j + 1].c == current && _A[i - 2][j + 2].c == current && _A[i - 3][j + 3].c == current && _A[i - 4][j + 4].c == current) return current;
			}
		}
	}

	// Kiểm tra hòa (hết ô trống)
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			if (_A[i][j].c == 0) return 2;

	return 0;
}

int CheckBoard(int pX, int pY) {
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (_A[i][j].x == pX && _A[i][j].y == pY && _A[i][j].c == 0) {
				if (_TURN == true) _A[i][j].c = -1; // Nếu lượt hiện hành là true thì c = -1
				else _A[i][j].c = 1; // Nếu lượt hiện hành là false thì c = 1
				return _A[i][j].c;
			}
		}
	}
	return 0;
}