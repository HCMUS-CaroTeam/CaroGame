#include "Control/name_input_data.h"
#include "Model/config.h"

/*
============================================================
NAME INPUT DATA - Định nghĩa mảng button cho màn hình nhập tên
============================================================

Quy tắc khai báo (giống menu_data.cpp):

Text button:
{ Vector2{x, y}, Vector2{w, h}, "TEXT", ID, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, fontSize, spacing }

Không khai báo icon button ở đây vì name_setup không dùng.
Các nút mũi tên avatar được xử lý thủ công bằng texture trong ui_name_setup.cpp
vì chúng gắn liền với vị trí avatar động - không phù hợp để đưa vào data tĩnh.
============================================================
*/

Button gNameInputButtons[] =
{
    // animIndex = NAME_INPUT_ANIM_PLAY (6)
    {
        Vector2{ SCREEN_WIDTH * 0.5f - 110.0f, 630.0f },
        Vector2{ 220.0f, 58.0f },
        "PLAY",
        NAME_INPUT_BTN_PLAY,
        BUTTON_VISUAL_TEXT,
        BUTTON_ICON_NONE,
        26.0f, 2.0f
    },
    // animIndex = NAME_INPUT_ANIM_BACK (7)
    {
        Vector2{ SCREEN_WIDTH * 0.5f - 110.0f, 780.0f },
        Vector2{ 220.0f, 58.0f },
        "BACK",
        NAME_INPUT_BTN_BACK,
        BUTTON_VISUAL_TEXT,
        BUTTON_ICON_NONE,
        26.0f, 2.0f
    }
};

int gNameInputButtonCount = sizeof(gNameInputButtons) / sizeof(gNameInputButtons[0]);