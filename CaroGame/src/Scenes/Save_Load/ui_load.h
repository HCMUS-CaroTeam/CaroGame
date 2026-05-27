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

extern char gLoadBuffer[32];         // Buffer chứa tên game load
extern int gLoadLetterCount;         // Độ dài tên game load hiện tại
extern const char* gLoadStatusMsg;   // Thông báo trạng thái load (Load thành công/thất bại)


void InitLoadUI();
void ShutdownLoadUI();
void UpdateLoadUI(
	const MouseState& mouse,
	float dt,
	AudioAssets& audio,
	const AppSettings& settings,
	ScreenState& currentScreen
);
void DrawLoadUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings);
