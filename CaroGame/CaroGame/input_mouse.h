#pragma once
#include "raylib.h"
// dữ liệu trạng thái chuột hiện tại
struct MouseState
{
    Vector2 position;
    bool leftPressed;
    bool leftDown;
    bool leftReleased;
};

// Lấy trạng thái chuột hiện tại
MouseState GetMouseStateNow();
// Kiểm tra xem chuột có đang ở trên button hay không
bool IsMouseOverRect(const MouseState& mouse, Rectangle rect);