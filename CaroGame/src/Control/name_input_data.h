#pragma once
#include "Control/menu_data.h"  // Dùng struct Button, enums - KHÔNG dùng mảng của menu_data

/*
============================================================
NAME INPUT DATA - Dữ liệu button riêng cho màn hình nhập tên
============================================================

File này làm:
- Khai báo enum ID riêng cho các nút của ui_name_setup
- Khai báo mảng button riêng (không đụng đến menu_data)
- Khai báo animIndex riêng, tránh conflict với các scene khác

File này KHÔNG làm:
- Không đụng đến gMainMenuButtons, gSetupButtons, hay bất kỳ
  mảng nào trong menu_data.cpp
- Không khai báo lại struct Button (dùng lại từ menu_data.h)

animIndex đang được dùng bởi các scene khác:
  0-4   → Main Menu
  6-7   → Name Input (file này)
  10-15 → Setup
  20-25 → Pause
  30    → Setting BACK
  31    → About BACK
  50-51 → Save
  50-52 → Notify
============================================================
*/

// ID riêng cho các nút của màn Name Input
enum NameInputButtonId
{
    NAME_INPUT_BTN_PLAY = 0,
    NAME_INPUT_BTN_BACK = 1
};

// animIndex dùng trong gWasHover/gHoverAnim/gPressAnim (mảng 128 phần tử)
// Chọn 6, 7 vì khoảng này chưa ai dùng
static constexpr int NAME_INPUT_ANIM_PLAY = 6;
static constexpr int NAME_INPUT_ANIM_BACK = 7;

// Khai báo extern - định nghĩa trong name_input_data.cpp
extern Button gNameInputButtons[];
extern int    gNameInputButtonCount;