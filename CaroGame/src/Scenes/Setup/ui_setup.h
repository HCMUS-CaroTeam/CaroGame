#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Control/menu_data.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"

void InitSetupUI();
void ShutdownSetupUI();

void UpdateSetupUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
);

void DrawSetupUI(Font fontTitle, Font fontSmall, const MouseState& mouse);