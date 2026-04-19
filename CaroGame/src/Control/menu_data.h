#pragma once
#include "raylib.h"
#include <string>
using namespace std;

enum ButtonVisualType
{
    BUTTON_VISUAL_TEXT = 0,
    BUTTON_VISUAL_ICON
};

enum ButtonIconType
{
    BUTTON_ICON_NONE = 0,
    BUTTON_ICON_ARROW_RIGHT,
    BUTTON_ICON_ARROW_LEFT
};

struct Button
{
    Vector2 position;
    Vector2 size;
    string title;
    int id;
    ButtonVisualType visualType;
    ButtonIconType iconType;

    float fontSize;
    float textSpacing;
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
    SCREEN_SETUP, 
	SCREEN_SAVE,
	SCREEN_LOAD
};

extern Button gMainMenuButtons[];
extern int gMainMenuButtonCount;

extern Button gSetupButtons[];
extern int gSetupButtonCount;

extern Button gPauseButtons[];
extern int gPauseButtonCount;

enum SettingButtonId
{
    SETTING_BTN_BACK = 300
};

extern Button gSettingButtons[];
extern int gSettingButtonCount;

enum AboutButtonId
{
    ABOUT_BTN_BACK = 400
};

extern Button gAboutButtons[];
extern int gAboutButtonCount;


enum SaveButtonId
{
    SAVE_BTN_CONFIRM = 0,
    SAVE_BTN_BACK
};