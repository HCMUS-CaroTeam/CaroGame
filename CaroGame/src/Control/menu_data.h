#pragma once
#include "raylib.h"
#include <string>
using namespace std;

struct Button
{
    Vector2 position;
    string title;
    int id;
};

enum MainMenuButtonId
{
    BTN_PLAY = 0,
    BTN_ABOUT,
    BTN_SETTING,
    BTN_EXIT
};

enum SetupButtonId
{
    SETUP_BTN_PLAY = 100,
	SETUP_BTN_PVP,
	SETUP_BTN_PVE,
    SETUP_BTN_PREV,
	SETUP_BTN_NEXT,
    SETUP_BTN_BACK
};

enum PauseButtonId
{
    PAUSE_BTN_CONTINUE = 200,
    PAUSE_BTN_SETTING,
    PAUSE_BTN_SAVE,
    PAUSE_BTN_LOAD,
    PAUSE_BTN_EXIT_MENU,
    PAUSE_BTN_EXIT_DESKTOP
};

enum ScreenState
{
    SCREEN_MAIN_MENU,
    SCREEN_PLAY,
    SCREEN_ABOUT,
    SCREEN_SETTING,
    SCREEN_SETUP
};

extern Button gMainMenuButtons[];
extern int gMainMenuButtonCount;

extern Button gSetupButtons[];
extern int gSetupButtonCount;

extern Button gPauseButtons[];
extern int gPauseButtonCount;