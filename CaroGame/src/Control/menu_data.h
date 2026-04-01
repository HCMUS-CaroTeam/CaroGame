#pragma once
#include "raylib.h"
#include <string>

struct NeonButton
{
    Vector2 position;
    std::string title;
    int id;
};

enum ButtonId
{
    BTN_PLAY = 0,
    BTN_ABOUT,
    BTN_SETTING,
    BTN_EXIT
};

enum ScreenState
{
    SCREEN_MAIN_MENU,
    SCREEN_PLAY,
    SCREEN_ABOUT,
    SCREEN_SETTING
};

extern NeonButton gButtons[];
extern int gButtonCount;