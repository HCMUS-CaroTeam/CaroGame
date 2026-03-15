#include "../include/Common.h"
#include "../include/View.h"
#include "../include/Model.h"
#include "../include/Control.h"

int main()
{
	FixConsoleWindow();
	StartGame(true);
	bool validEnter = true;
	while (true)
	{
		GotoXY(_X, _Y); // Đảm bảo con trỏ luôn ở vị trí hiện hành trên bàn cờ
		_COMMAND = toupper(_getch());
		if (_COMMAND == 27)
		{
			ExitGame();
			return 0;
		}
		else
		{
			if (_COMMAND == 'A')
				MoveLeft();
			else if (_COMMAND == 'W')
				MoveUp();
			else if (_COMMAND == 'S')
				MoveDown();
			else if (_COMMAND == 'D')
				MoveRight();
			else if (_COMMAND == 13)
			{ // Người dùng đánh dấu trên màn hình bàn cờ
				switch (CheckBoard())
				{
				case -1:
					cout << "X";
					break;
				case 1:
					cout << "O";
					break;
				case 0:
					validEnter = false; // Khi đánh vào ô đã đánh rồi
				}
				// Tiếp theo là kiểm tra và xử lý thắng/thua/hòa/tiếp tục
				if (validEnter == true)
				{
					switch (ProcessFinish(TestBoard()))
					{
					case -1:
					case 1:
					case 0:
						if (AskContinue() != 'Y')
						{
							ExitGame();
							return 0;
						}
						else
							StartGame();
					}
				}
				validEnter = true; // Mở khóa
			}
		}
	}
}