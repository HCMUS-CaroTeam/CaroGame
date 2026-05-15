#include "ui_notify.h"
#include "Model/logic.h"
#include "Scenes/Save_Load/ui_save.h"

static constexpr float SAVE_PANEL_W = 1000.0f;
static constexpr float SAVE_PANEL_H = 300.0f;

static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color)
{
	if (text == nullptr || text[0] == '\0') return;
	Vector2 sz = MeasureTextEx(font, text, fontSize, 1.0f);
	DrawTextEx(font, text,
		Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
		fontSize, 1.0f, color);
}

void InitNotifyUI() {
	// Khởi tạo trạng thái nếu cần
}

void ShutdownNotifyUI() {
	// Giải phóng tài nguyên nếu cần
}

void UpdateNotifyUI(
	const MouseState& mouse,
	float dt,
	AudioAssets& audio,
	const AppSettings& settings,
	ScreenState& currentScreen,
	bool& shouldClose,
	bool isShouldClose
) {
	// Xử lý nút bấm
	if (current().nameGame[0] == '\0')
	{
		for (int i = 0; i < gNotifyButtonCount; ++i)
		{
			bool hovered = false, pressed = false;
			UpdateUIButton(50 + i, gNotifyButtons[i], mouse, dt, audio, settings, hovered, pressed);
			if (hovered && mouse.leftPressed)
			{
				PlayMenuClick(audio, settings);
				if (gNotifyButtons[i].id == NOTIFY_BTN_CONFIRM_YES)
				{
					if (isShouldClose) {
						currentScreen = SCREEN_SAVE_TO_EXIT;
					}
					else {
						currentScreen = SCREEN_SAVE_TO_BACK_MENU;
					}
				}
				else if (gNotifyButtons[i].id == NOTIFY_BTN_CONFIRM_NO)
				{
					if (isShouldClose) {
						shouldClose = true; // Đặt cờ để thoát game mà không lưu
					}
					else {
						InitNewGame(); // Bắt đầu game mới mà không lưu
						InitSaveUI(); 
						currentScreen = SCREEN_MAIN_MENU; // Quay lại menu chính nếu không muốn lưu
					}
				}
				else if (gNotifyButtons[i].id == NOTIFY_BTN_BACK)
				{
					currentScreen = SCREEN_PLAY; // Quay lại chơi nếu không muốn lưu
				}
			}
		}
	}
	else if (current().nameGame[0] != '\0') {
		SaveData(current()); // Tự động lưu nếu đã có tên game, không cần hỏi lại
		if (isShouldClose) {
			shouldClose = true; // Đặt cờ để thoát game sau khi tự động lưu
		}
		else {
			InitNewGame(); // Bắt đầu game mới sau khi tự động lưu
			InitSaveUI();
			currentScreen = SCREEN_MAIN_MENU; // Quay lại menu chính sau khi tự động lưu
		}

	}

	if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_PLAY;
}

void DrawNotifyUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings) {
	DrawBackgroundOnly();
	// Vẽ Panel chính
	Rectangle panel = {
		SCREEN_WIDTH * 0.5f - SAVE_PANEL_W * 0.5f,
		SCREEN_HEIGHT * 0.5f - SAVE_PANEL_H * 0.5f,
		SAVE_PANEL_W, SAVE_PANEL_H
	};
	DrawPanelFrame(panel);
	// Thông báo
	const char* msg = "Do you want to save your progress?";
	DrawCenteredText(fontTitle, msg, panel.y + 50.0f, 28.0f, LIME);
	// Hướng dẫn
	DrawCenteredText(fontSmall, "Press YES (NO / BACK) to SAVE (DON'T SAVE / BACK TO GAME),", panel.y + 100.0f, 20.0f, LIGHTGRAY);
	// Vẽ các nút bấm
	for (int i = 0; i < gNotifyButtonCount; ++i)
	{
		Rectangle hitRect = GetButtonRect(gNotifyButtons[i]);
		bool hov = IsMouseOverRect(mouse, hitRect);
		bool prs = hov && mouse.leftDown;
		DrawUIButton(50 + i, gNotifyButtons[i], fontSmall, hov, prs);
	}
}