//#pragma once
//#include "View/ui_background.h"
//#include "View/ui_button.h"
//#include "View/ui_frame.h"
//#include "Model/config.h"
//#include "Model/game_data.h"
//#include "Systems/save.h"
//#include <cstring>
//
//extern char gInputBuffer[32]; // Buffer chứa tên save
//extern int gLetterCount;               // Độ dài tên hiện tại
//extern const char* gStatusMsg;        // Thông báo trạng thái (Lưu thành công/thất bại)
//
//extern Button gSaveButtons[];
//extern int gSaveButtonCount;
//
//static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color);
//
//void InitSaveUI();
//
//void ShutdownSaveUI();
//
//void UpdateSaveUI(
//    const MouseState& mouse,
//    float dt,
//    AudioAssets& audio,
//    const AppSettings& settings,
//    ScreenState& currentScreen
//);
//
//void DrawSaveUI(Font fontTitle, Font fontSmall, const MouseState& mouse);