#include "Control/menu_data.h"

// Kích thước mỗi nút: 280 x 80 px
// Màn hình: 1600 x 900
// Bố cục 2 cột x 2 hàng, căn giữa màn hình:
//   Tổng chiều ngang 2 nút + khoảng cách: 280 + 40 + 280 = 600
//   Cột trái  x = 1600/2 - 300 = 500
//   Cột phải  x = 1600/2 +  20 = 820
//   Hàng 1    y = 460
//   Hàng 2    y = 460 + 80 + 30 = 570

NeonButton gButtons[] =
{
    { Vector2{ 500.0f, 460.0f }, "PLAY",    BTN_PLAY    },
    { Vector2{ 820.0f, 460.0f }, "ABOUT",   BTN_ABOUT   },
    { Vector2{ 500.0f, 570.0f }, "SETTING", BTN_SETTING },
    { Vector2{ 820.0f, 570.0f }, "EXIT",    BTN_EXIT    }
};

int gButtonCount = sizeof(gButtons) / sizeof(gButtons[0]);