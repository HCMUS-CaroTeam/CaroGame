#include "menu_data.h"
#include "colors.h"
// Dữ liệu cho các nút trong menu chính
NeonButton gButtons[] =
{
    { Rectangle{470, 220, 350, 100}, "INSTANT MATCH",      "RANDOM",                  COLOR_NEON_CYAN, BTN_INSTANT_MATCH },
    { Rectangle{860, 220, 350, 100}, "LINK-UP: VS FRIEND", "LOCAL / MULTIPLAYER", COLOR_NEON_CYAN, BTN_VS_FRIEND   },
    { Rectangle{470, 380, 350, 100}, "LINK-UP: VS AI",     "SINGLE PLAYER",     COLOR_NEON_PINK, BTN_VS_AI       },
    { Rectangle{860, 380, 350, 100}, "USER PROFILE",       "& DATA",            COLOR_NEON_PINK, BTN_USER_PROFILE},
    { Rectangle{470, 540, 350, 100}, "CHALLENGE ARCHIVES", "PUZZLES / SCENES",  COLOR_NEON_CYAN, BTN_CHALLENGE   },
    { Rectangle{860, 540, 350, 100}, "SETTING",      "& DATA",            COLOR_NEON_CYAN, BTN_CONFIG      },
    { Rectangle{1240,650,110, 70},   "EXIT",               "",                  COLOR_NEON_PINK, BTN_EXIT        }
};

// Số lượng nút trong menu
int gButtonCount = sizeof(gButtons) / sizeof(gButtons[0]);