#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"
#include "Control/menu_data.h"

void InitSettingUI();
void ShutdownSettingUI();

// Gọi trước khi chuyển sang SCREEN_SETTING để xác định màn hình sẽ Back về
void SetSettingReturnScreen(ScreenState from);

void UpdateSettingUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    AppSettings& settings,
    ScreenState& currentScreen
);

void DrawSettingUI(
    Font fontTitle,
    Font fontSmall,
    Font fontMini,
    const MouseState& mouse,
    const AppSettings& settings
);
