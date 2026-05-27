#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Systems/audio_manager.h"
#include "Control/menu_data.h"
#include "Model/app_settings.h"

void InitMainMenuUI();
void ShutdownMainMenuUI();

void UpdateMainMenuUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
);

void DrawMainMenuUI(Font fontTitle, Font fontSmall, const MouseState& mouse);