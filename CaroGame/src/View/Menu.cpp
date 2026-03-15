
#include "../../include/View.h"

void FixConsoleWindow()
{
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void GotoXY(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int AskContinue()
{
	GotoXY(0, TOP + BOARD_SIZE * 2 + 4); // Di chuyển con trỏ xuống dưới kết quả để hỏi người dùng
	cout << "Nhan 'Y' de tiep tuc, phim bat ky de thoat: ";
	return toupper(_getch());
}