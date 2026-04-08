#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Control/menu_data.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"

void InitUIButtonSystem();
void ShutdownUIButtonSystem();

Rectangle GetButtonRect(const Button& button);

void UpdateUIButton(
    int index,
    const Button& button,
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    bool& hovered,
    bool& pressed
);

void DrawUIButton(
    int index,
    const Button& button,
    Font fontTitle,
    bool hovered,
    bool pressed
);