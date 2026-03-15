#include "../../include/View.h"
extern void DrawBoard(int pSize)
{
	for (int i = 0; i <= pSize; i++)
	{
		for (int j = 0; j <= pSize; j++)
		{
			GotoXY(LEFT + 4 * i, TOP + 2 * j);
			cout << ".";
		}
	}
	GotoXY(_X, _Y); //	<?xml version="1.0" encoding="utf-8"?>

}

// Qui ước: -1 = X thắng, 1 = O thắng, 0 = hòa, 2 = chưa hết game
int ProcessFinish(int pWhoWin)
{
	GotoXY(0, TOP + 2 * BOARD_SIZE + 2); // Di chuyển con trỏ xuống dưới bàn cờ để in kết quả
	switch (pWhoWin)
	{
	case -1:
		cout << "Nguoi choi X thang!       " << endl;
		XScore++;
		NumberOfRounds++;
		break;
	case 1:
		cout << "Nguoi choi O thang!       " << endl;
		OScore++;
		NumberOfRounds++;
		break;
	case 0:
		cout << "Hoa nhau!                 " << endl;
		NumberOfRounds++;
		break;
	case 2:
		_TURN = !_TURN;
		GotoXY(_X, _Y);
		return 2;
	}
	 // Trả về vị trí hiện hành của con trỏ màn hình bàn cờ
	return pWhoWin;
}