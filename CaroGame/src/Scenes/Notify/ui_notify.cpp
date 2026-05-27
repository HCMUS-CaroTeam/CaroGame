#include "ui_notify.h"
#include "Model/logic.h"
#include "Scenes/Save_Load/ui_save.h"

static constexpr float SAVE_PANEL_W = 1000.0f;
static constexpr float SAVE_PANEL_H = 360.0f;

static Button GetNotifyActionButton(int index)
{
	switch (index)
	{
	case 0:
		return Button{ Vector2{ SCREEN_WIDTH * 0.5f - 390.0f, SCREEN_HEIGHT * 0.5f + 82.0f },
					   Vector2{ 240.0f, 58.0f }, "YES", NOTIFY_BTN_CONFIRM_YES,
					   BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 24.0f, 1.5f };
	case 1:
		return Button{ Vector2{ SCREEN_WIDTH * 0.5f - 120.0f, SCREEN_HEIGHT * 0.5f + 82.0f },
					   Vector2{ 240.0f, 58.0f }, "NO", NOTIFY_BTN_CONFIRM_NO,
					   BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 24.0f, 1.5f };
	default:
		return Button{ Vector2{ SCREEN_WIDTH * 0.5f + 150.0f, SCREEN_HEIGHT * 0.5f + 82.0f },
					   Vector2{ 240.0f, 58.0f }, "BACK", NOTIFY_BTN_BACK,
					   BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 24.0f, 1.5f };
	}
}

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
			Button button = GetNotifyActionButton(i);
			bool hovered = false, pressed = false;
			UpdateUIButton(50 + i, button, mouse, dt, audio, settings, hovered, pressed);
			if (hovered && mouse.leftPressed)
			{
				PlayMenuClick(audio, settings);
				if (button.id == NOTIFY_BTN_CONFIRM_YES)
				{
					if (isShouldClose) {
						currentScreen = SCREEN_SAVE_TO_EXIT;
					}
					else {
						currentScreen = SCREEN_SAVE_TO_BACK_MENU;
					}
				}
				else if (button.id == NOTIFY_BTN_CONFIRM_NO)
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
				else if (button.id == NOTIFY_BTN_BACK)
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
	(void)settings;
	DrawBackgroundOnly();
	// Vẽ Panel chính
	Rectangle panel = {
		SCREEN_WIDTH * 0.5f - SAVE_PANEL_W * 0.5f,
		SCREEN_HEIGHT * 0.5f - SAVE_PANEL_H * 0.5f,
		SAVE_PANEL_W, SAVE_PANEL_H
	};
	DrawPanelFrame(panel);
	// Thông báo
	Rectangle content = {
		panel.x + 70.0f,
		panel.y + 42.0f,
		panel.width - 140.0f,
		150.0f
	};
	DrawRectangleRounded(content, 0.12f, 10, Color{ 34, 26, 38, 150 });
	DrawRectangleRoundedLinesEx(content, 0.12f, 10, 2.0f, Color{ 255, 214, 94, 110 });
	DrawCenteredText(fontTitle, "SAVE PROGRESS?", panel.y + 70.0f, 38.0f, Color{ 255, 236, 194, 255 });
	// Hướng dẫn
	DrawCenteredText(fontSmall, "YES saves before leaving. NO leaves without saving.", panel.y + 128.0f, 22.0f, Color{ 235, 231, 216, 235 });
	DrawCenteredText(fontSmall, "BACK returns to the match.", panel.y + 162.0f, 22.0f, Color{ 190, 226, 255, 230 });
	// Vẽ các nút bấm
	for (int i = 0; i < gNotifyButtonCount; ++i)
	{
		Button button = GetNotifyActionButton(i);
		Rectangle hitRect = GetButtonRect(button);
		bool hov = IsMouseOverRect(mouse, hitRect);
		bool prs = hov && mouse.leftDown;
		DrawUIButton(50 + i, button, fontSmall, hov, prs);
	}
}
