#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Control/menu_data.h"
#include "Systems/audio_manager.h"
#include "Systems/save_load.h"
#include "Model/app_settings.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/config.h"
#include "Model/game_data.h"

extern char gInputBuffer[32];         // Buffer chứa tên save
extern int gLetterCount;              // Độ dài tên hiện tại
extern const char* gStatusMsg;        // Thông báo trạng thái (Lưu thành công/thất bại)

void InitSaveUI();
void ShutdownSaveUI();
// For SCREEN_SAVE_FIRST: Lưu game mới (bắt buộc nhập tên mới)
void UpdateSaveUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
);
void DrawSaveUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings);

// For SCREEN_SAVE_SECOND: Lưu game đã có sẵn (cập nhật game cũ, không nhập tên mới)
void UpdateSaveUISecond(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
);

// For SCREEN_SAVE_AS: Lưu game với tên mới (cập nhật game cũ, cho phép nhập tên mới nhưng không bắt buộc).
void UpdateSaveAsUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
);
void DrawSaveAsUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings);

void UpdateSaveToBackMenuUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
);

void UpdateSaveToExitUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
	bool& shouldClose
);
