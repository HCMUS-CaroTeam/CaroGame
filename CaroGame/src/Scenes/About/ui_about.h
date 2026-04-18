#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"
#include "Control/menu_data.h"

void InitAboutUI();
void ShutdownAboutUI();

void UpdateAboutUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
);

void DrawAboutUI(
    Font fontTitle,
    Font fontSmall,
    const MouseState& mouse
);
