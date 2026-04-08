#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "audio_manager.h"
#include "Model/app_settings.h"
#include "Control/menu_data.h"

void InitPauseUI();
void ShutdownPauseUI();

void TogglePause();
void ClosePause();
bool IsPauseActive();

void UpdatePauseUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
);

void DrawPauseUI(Font fontTitle, Font fontSmall, const MouseState& mouse);