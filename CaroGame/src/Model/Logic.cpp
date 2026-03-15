#include "../../include/Model.h"

int TestBoard()
{
	bool hasEmptyCell = false;

	for (int i = 0; i < BOARD_SIZE; i++)
	{
		for (int j = 0; j < BOARD_SIZE; j++)
		{
			if (_BOARD[i][j] != 0) // Nếu ô có quân cờ
			{
				char current = _BOARD[i][j];

				// 1. Kiểm tra hàng ngang
				if (j <= BOARD_SIZE - 5 &&
					_BOARD[i][j + 1] == current && _BOARD[i][j + 2] == current &&
					_BOARD[i][j + 3] == current && _BOARD[i][j + 4] == current)
					return current;

				// 2. Kiểm tra hàng dọc
				if (i <= BOARD_SIZE - 5 &&
					_BOARD[i + 1][j] == current && _BOARD[i + 2][j] == current &&
					_BOARD[i + 3][j] == current && _BOARD[i + 4][j] == current)
					return current;

				// 3. Kiểm tra chéo xuôi (\)
				if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5 &&
					_BOARD[i + 1][j + 1] == current && _BOARD[i + 2][j + 2] == current &&
					_BOARD[i + 3][j + 3] == current && _BOARD[i + 4][j + 4] == current)
					return current;

				// 4. Kiểm tra chéo ngược (/)
				if (i >= 4 && j <= BOARD_SIZE - 5 &&
					_BOARD[i - 1][j + 1] == current && _BOARD[i - 2][j + 2] == current &&
					_BOARD[i - 3][j + 3] == current && _BOARD[i - 4][j + 4] == current)
					return current;
			}
			else
			{
				hasEmptyCell = true; // Ghi nhận vẫn còn ô trống
			}
		}
	}

	// Nếu không ai thắng và vẫn còn ô trống -> Tiếp tục chơi
	if (hasEmptyCell)
		return 2;

	// Nếu không ai thắng và hết ô trống -> Hòa
	return 0;
}

int CheckBoard() // Không cần truyền pX, pY nữa vì đã dùng _ROW, _COL toàn cục
{
	// Kiểm tra xem ô tại vị trí con trỏ hiện tại có trống không
	if (_BOARD[_ROW][_COL] == 0)
	{
		if (_TURN == true)
			_BOARD[_ROW][_COL] = -1; // X đánh
		else
			_BOARD[_ROW][_COL] = 1; // O đánh

		return _BOARD[_ROW][_COL]; // Trả về giá trị vừa đánh (-1 hoặc 1)
	}
	return 0; // Ô đã có quân, không cho đánh
}