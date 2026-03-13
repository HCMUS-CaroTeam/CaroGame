#include "../../include/View.h"

void DrawBoard(int pSize) {
	for (int i = 0; i <= pSize; i++) {
		for (int j = 0; j <= pSize; j++) {
			GotoXY(LEFT + 4 * i, TOP + 2 * j);
			cout << ".";
		}
	}
	GotoXY(_X, _Y); //	<?xml version="1.0" encoding="utf-8"?>
	
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