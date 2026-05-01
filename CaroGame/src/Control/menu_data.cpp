#include "Control/menu_data.h"
#include "Model/config.h"

/*
============================================================
HUONG DAN KHAI BAO BUTTON TRONG PROJECT
============================================================

1. Cau truc Button

struct Button
{
    Vector2 position;         // toa do goc trai tren cua nut
    Vector2 size;             // kich thuoc nut
    string title;             // text hien thi, icon button thi de ""
    int id;                   // ma nut de xu ly click
    ButtonVisualType visualType; // loai nut: text hoac icon
    ButtonIconType iconType;     // loai icon, text button thi de BUTTON_ICON_NONE
};

------------------------------------------------------------
2. Template TEXT BUTTON
------------------------------------------------------------

{ Vector2{ posX, posY }, Vector2{ width, height }, "TEXT", BUTTON_ID, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE }

Vi du:
{ Vector2{ 205.0f, 690.0f }, Vector2{ 280.0f, 80.0f }, "PLAY", BTN_PLAY, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE }

------------------------------------------------------------
3. Template ICON BUTTON
------------------------------------------------------------

{ Vector2{ posX, posY }, Vector2{ width, height }, "", BUTTON_ID, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_LEFT }

hoac

{ Vector2{ posX, posY }, Vector2{ width, height }, "", BUTTON_ID, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_RIGHT }

Vi du:
{ Vector2{ SCREEN_WIDTH * 0.5f - 220.0f, 560.0f }, Vector2{ 64.0f, 64.0f }, "", SETUP_BTN_ARROW_LEFT, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_LEFT }

{ Vector2{ SCREEN_WIDTH * 0.5f + 156.0f, 560.0f }, Vector2{ 64.0f, 64.0f }, "", SETUP_BTN_ARROW_RIGHT, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_RIGHT }

------------------------------------------------------------
4. Quy tac dung
------------------------------------------------------------

- Text button:
  + có chữ
  + visualType = BUTTON_VISUAL_TEXT
  + iconType = BUTTON_ICON_NONE

- Icon button:
  + không có chữ
  + title = ""
  + visualType = BUTTON_VISUAL_ICON
  + iconType phải chọn đúng mũi tên trái / phải

- Hitbox và ảnh đều lấy theo:
  + position
  + size
- size tự chỉnh cho từng nút
- fontSize tự chỉnh cho từng nút
- textSpacing tự chỉnh cho từng nút
- icon button thì để fontSize = 0, textSpacing = 0
=> chỉ cần khai báo đúng position và size là vùng bấm và vùng vẽ sẽ đồng bộ.
------------------------------------------------------------
5. Mẫu khai báo cả mảng
------------------------------------------------------------

Button gExampleButtons[] =
{
    { Vector2{ 200.0f, 700.0f }, Vector2{ 280.0f, 80.0f }, "PLAY", BTN_PLAY, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE },
    { Vector2{ 500.0f, 700.0f }, Vector2{ 280.0f, 80.0f }, "BACK", BTN_BACK, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE },
    { Vector2{ 900.0f, 560.0f }, Vector2{ 64.0f, 64.0f }, "", BTN_LEFT, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_LEFT },
    { Vector2{ 980.0f, 560.0f }, Vector2{ 64.0f, 64.0f }, "", BTN_RIGHT, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_RIGHT }
};

int gExampleButtonCount = sizeof(gExampleButtons) / sizeof(gExampleButtons[0]);

------------------------------------------------------------
6. Tom tat nhanh
------------------------------------------------------------

Text button:
{ Vector2{ x, y }, Vector2{ w, h }, "TEXT", ID, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, fontSize, textSpacing }

Icon button:
{ Vector2{ x, y }, Vector2{ w, h }, "", ID, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_LEFT, fontSize, textSpacing }

============================================================
*/

// Main Menu Buttons
Button gMainMenuButtons[] =
{
    { Vector2{ 205.0f, 690.0f },  Vector2{ 280.0f, 80.0f }, "PLAY",    BTN_PLAY,    BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f },
    { Vector2{ 510.0f, 690.0f },  Vector2{ 280.0f, 80.0f }, "ABOUT",   BTN_ABOUT,   BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f },
    { Vector2{ 815.0f, 690.0f },  Vector2{ 280.0f, 80.0f }, "SETTING", BTN_SETTING, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f },
    { Vector2{ 1120.0f, 690.0f }, Vector2{ 280.0f, 80.0f }, "EXIT",    BTN_EXIT,    BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f }
};

int gMainMenuButtonCount = sizeof(gMainMenuButtons) / sizeof(gMainMenuButtons[0]);

Button gSetupButtons[] =
{
    { Vector2{ 1050.0f, 250.0f }, Vector2{ 200.0f, 60.0f }, "PVP", SETUP_BTN_PVP, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f },
    { Vector2{ 1050.0f, 340.0f }, Vector2{ 200.0f, 60.0f }, "PVE", SETUP_BTN_PVE, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f },

    // Nút Play ở giữa
    { Vector2{ 550.0f, 570.0f }, Vector2{ 200.0f, 60.0f }, "PLAY", SETUP_BTN_PLAY, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f },

    // Nút mũi tên trái / phải
    { Vector2{ 130.0f, 300.0f }, Vector2{ 300.0f, 300.0f }, "", SETUP_BTN_PREV, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_LEFT, 0.0f, 0.0f },
    { Vector2{ 890.0f, 360.0f }, Vector2{ 300.0f, 300.0f }, "", SETUP_BTN_NEXT, BUTTON_VISUAL_ICON, BUTTON_ICON_ARROW_RIGHT, 0.0f, 0.0f },

    { Vector2{ 1050.0f, 570.0f }, Vector2{ 200.0f, 60.0f }, "BACK", SETUP_BTN_BACK, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 28.0f, 2.0f }
};
int gSetupButtonCount = sizeof(gSetupButtons) / sizeof(gSetupButtons[0]);

Button gPauseButtons[] =
{
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 270.0f }, Vector2{ 320.0f, 50.0f }, "CONTINUE",        PAUSE_BTN_CONTINUE,     BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 327.0f }, Vector2{ 320.0f, 50.0f }, "SETTING",         PAUSE_BTN_SETTING,      BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 384.0f }, Vector2{ 320.0f, 50.0f }, "SAVE",            PAUSE_BTN_SAVE,         BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 441.0f }, Vector2{ 320.0f, 50.0f }, "SAVE AS",         PAUSE_BTN_SAVE_AS,      BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 498.0f }, Vector2{ 320.0f, 50.0f }, "LOAD",            PAUSE_BTN_LOAD,         BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 555.0f }, Vector2{ 320.0f, 50.0f }, "EXIT TO MENU",    PAUSE_BTN_EXIT_MENU,    BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 612.0f }, Vector2{ 320.0f, 50.0f }, "EXIT TO DESKTOP", PAUSE_BTN_EXIT_DESKTOP, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f }
};

int gPauseButtonCount = sizeof(gPauseButtons) / sizeof(gPauseButtons[0]);

Button gSettingButtons[] =
{
    { Vector2{ 660.0f, 710.0f }, Vector2{ 280.0f, 62.0f }, "BACK", SETTING_BTN_BACK, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 26.0f, 2.0f }
};

int gSettingButtonCount = sizeof(gSettingButtons) / sizeof(gSettingButtons[0]);

// animIndex = 31
Button gAboutButtons[] =
{
    { Vector2{ 660.0f, 715.0f }, Vector2{ 280.0f, 62.0f }, "BACK", ABOUT_BTN_BACK, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 26.0f, 2.0f }
};

int gAboutButtonCount = sizeof(gAboutButtons) / sizeof(gAboutButtons[0]);

Button gSaveButtons[] =
{
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 480.0f }, Vector2{ 320.0f, 60.0f }, "CONFIRM SAVE", SAVE_BTN_CONFIRM, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 24.0f, 1.5f},
    { Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 560.0f }, Vector2{ 320.0f, 60.0f }, "BACK TO GAME", SAVE_BTN_BACK, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 24.0f, 1.5f}
};

int gSaveButtonCount = sizeof(gSaveButtons) / sizeof(gSaveButtons[0]);


// Các 
float previewX = SCREEN_WIDTH * 0.5f + 180.0f;
float previewY = 160.0f;
float btnW = 260.0f; // Chiều rộng nút trong khung preview
float btnH = 45.0f;  // Chiều cao nút

Button gLoadButtons[] = {
    // 4 nút hiển thị thông số (Visual Text)
    { { previewX + 20, previewY + 70 },  { btnW, btnH }, "", LOAD_BTN_GN,    BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.2f },
    { { previewX + 20, previewY + 130 }, { btnW, btnH }, "", LOAD_BTN_GP1,   BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.2f },
    { { previewX + 20, previewY + 190 }, { btnW, btnH }, "", LOAD_BTN_GP2,   BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.2f },
    { { previewX + 20, previewY + 250 }, { btnW, btnH }, "", LOAD_BTN_GMODE, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.2f },

    // Nút chức năng ở dưới cùng
    { { previewX + 20, previewY + 330 }, { btnW, 50.0f }, "CONFIRM", LOAD_BTN_CONFIRM, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 22.0f, 1.5f },
    { { SCREEN_WIDTH * 0.5f - 160.0f, 560.0f }, { 320.0f, 60.0f }, "BACK TO MENU", LOAD_BTN_BACK, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 24.0f, 1.5f }
};