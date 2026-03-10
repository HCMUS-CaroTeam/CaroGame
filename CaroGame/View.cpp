#include "View.h"

// --- Copy nội dung 5 hàm FixConsoleWindow, GotoXY, DrawBoard, ProcessFinish, AskContinue từ source cũ vào đây ---

void FixConsoleWindow() {
	HWND consoleWindow = GetConsoleWindow();
	LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void GotoXY(int x, int y) {
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void DrawBoard(int pSize) {
	for (int i = 0; i <= pSize; i++) {
		for (int j = 0; j <= pSize; j++) {
			GotoXY(LEFT + 4 * i, TOP + 2 * j);
			cout << ".";
		}
	}
	GotoXY(_X, _Y); // ?
}

// Qui ước: -1 = X thắng, 1 = O thắng, 0 = hòa, 2 = chưa hết game
int ProcessFinish(int pWhoWin) {
	GotoXY(0, _A[BOARD_SIZE - 1][0].y + 2);
	switch (pWhoWin) {
	case -1: cout << "Nguoi choi X thang!       " << endl; break;
	case 1:  cout << "Nguoi choi O thang!       " << endl; break;
	case 0:  cout << "Hoa nhau!                 " << endl; break;
	case 2:  _TURN = !_TURN; return 2;
	}
	GotoXY(_X, _Y); // Trả về vị trí hiện hành của con trỏ màn hình bàn cờ
	return pWhoWin;
}

int AskContinue() {
	GotoXY(0, _A[BOARD_SIZE - 1][BOARD_SIZE - 1].y + 4);
	cout << "Nhan 'Y' de tiep tuc, phim bat ky de thoat: ";
	return toupper(_getch());
}