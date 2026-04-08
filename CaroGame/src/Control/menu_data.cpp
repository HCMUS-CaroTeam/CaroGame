#include "Control/menu_data.h"
#include "Model/config.h"

Button gMainMenuButtons[] =
{
    { Vector2{ 205.0f, 690.0f }, "PLAY",    BTN_PLAY    },
    { Vector2{ 510.0f, 690.0f }, "ABOUT",   BTN_ABOUT   },
    { Vector2{ 815.0f, 690.0f }, "SETTING", BTN_SETTING },
    { Vector2{ 1120.0f, 690.0f }, "EXIT",   BTN_EXIT    }
};

int gMainMenuButtonCount = sizeof(gMainMenuButtons) / sizeof(gMainMenuButtons[0]);

Button gSetupButtons[] =
{
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 690.0f }, "PLAY", SETUP_BTN_PLAY },
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 770.0f }, "BACK", SETUP_BTN_BACK }
};

int gSetupButtonCount = sizeof(gSetupButtons) / sizeof(gSetupButtons[0]);

Button gPauseButtons[] =
{
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 270.0f }, "CONTINUE",        PAUSE_BTN_CONTINUE     },
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 340.0f }, "SETTING",         PAUSE_BTN_SETTING      },
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 410.0f }, "SAVE",            PAUSE_BTN_SAVE         },
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 480.0f }, "LOAD",            PAUSE_BTN_LOAD         },
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 550.0f }, "EXIT TO MENU",    PAUSE_BTN_EXIT_MENU    },
    { Vector2{ SCREEN_WIDTH * 0.5f - 140.0f, 620.0f }, "EXIT TO DESKTOP", PAUSE_BTN_EXIT_DESKTOP }
};

int gPauseButtonCount = sizeof(gPauseButtons) / sizeof(gPauseButtons[0]);