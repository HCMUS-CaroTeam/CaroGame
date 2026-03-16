#include "../../include/View.h"
#include <raylib.h>

void FixConsoleWindow()
{
	//HWND consoleWindow = GetConsoleWindow();
	//LONG style = GetWindowLong(consoleWindow, GWL_STYLE);
	//style = style & ~(WS_MAXIMIZEBOX) & ~(WS_THICKFRAME);
	//SetWindowLong(consoleWindow, GWL_STYLE, style);
}

void GotoXY(int x, int y)
{
	//COORD coord;
	//coord.X = x;
	//coord.Y = y;
	//SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

int AskContinue() {
	DrawText("Ban muon choi tiep? (Y/N)", 250, 500, 20, DARKGRAY);

	if (IsKeyPressed(KEY_Y)) return 'Y';
	if (IsKeyPressed(KEY_N) || IsKeyPressed(KEY_ESCAPE)) return 'N';

	return 0; // Trả về 0 để vòng lặp tiếp tục chạy cho đến khi nhấn phím
}

void DrawMainMenu(int selectedOption) {
	ClearBackground(RAYWHITE);
	DrawText("CARO GAME", 280, 100, 50, DARKBLUE);

	// Danh sách các lựa chọn
	const char* options[] = { "NEW GAME", "LOAD GAME", "SETTINGS", "EXIT" };

	for (int i = 0; i < 4; i++) {
		Color color = (i == selectedOption) ? RED : BLACK; // Đổi màu khi chọn
		int fontSize = (i == selectedOption) ? 30 : 25;

		DrawText(options[i], 320, 220 + i * 50, fontSize, color);

		// Vẽ mũi tên chỉ vào lựa chọn đang được chọn
		if (i == selectedOption) {
			DrawText(">", 290, 220 + i * 50, 30, RED);
		}
	}
}

void DrawPlayerStats() {
	// Vẽ khung bảng điểm
	DrawRectangleLines(600, 100, 180, 250, BLACK);
	DrawText("BANG DIEM", 635, 120, 20, MAROON);

	// Hiển thị điểm số
	DrawText(TextFormat("Player X: %d", XScore), 620, 170, 20, BLUE);
	DrawText(TextFormat("Player O: %d", OScore), 620, 210, 20, RED);

	// Hiển thị lượt đi hiện tại
	DrawText("LUOT DI:", 620, 270, 18, DARKGRAY);
	if (_TURN)
		DrawText("PLAYER X", 620, 300, 22, BLUE);
	else
		DrawText("PLAYER O", 620, 300, 22, RED);
}