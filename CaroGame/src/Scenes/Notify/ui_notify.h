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
#include "Scenes/Save_Load/ui_save.h"
#include "Scenes/Save_Load/ui_load.h"

void InitNotifyUI();
void ShutdownNotifyUI();

void UpdateNotifyUI(
	const MouseState& mouse,
	float dt,
	AudioAssets& audio,
	const AppSettings& settings,
	ScreenState& currentScreen,
	bool& shouldClose,
	bool isShouldClose
);

void DrawNotifyUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings);