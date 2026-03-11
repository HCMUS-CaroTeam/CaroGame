#pragma once
#include "raylib.h"
#include "menu_data.h"
#include "input_mouse.h"
// Giao diện vẽ nút trong menu chính
bool IsButtonHovered(const NeonButton& btn, const MouseState& mouse);
// Kiểm tra xem nút có đang được click hay không
bool IsButtonClicked(const NeonButton& btn, const MouseState& mouse);
// Vẽ một nút với hiệu ứng neon, dựa trên trạng thái hover và click
void DrawNeonButton(
    const NeonButton& btn,
    bool forceFocus,
    const MouseState& mouse,
    float hoverAnim,
    float clickAnim,
    Font fontTitle,
    Font fontSub
);