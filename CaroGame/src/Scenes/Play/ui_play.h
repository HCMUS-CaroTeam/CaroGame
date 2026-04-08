#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Control/menu_data.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"

void InitPlayUI();
void ShutdownPlayUI();

void UpdatePlayUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
);

void DrawPlayUI(Font fontTitle, Font fontSmall, const MouseState& mouse);