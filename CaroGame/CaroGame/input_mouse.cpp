#include "input_mouse.h"


MouseState GetMouseStateNow()
{
    MouseState mouse{};
    mouse.position = GetMousePosition();
    mouse.leftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    mouse.leftDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    mouse.leftReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    return mouse;
}

bool IsMouseOverRect(const MouseState& mouse, Rectangle rect)
{
    return (mouse.position.x >= rect.x &&
        mouse.position.x <= rect.x + rect.width &&
        mouse.position.y >= rect.y &&
        mouse.position.y <= rect.y + rect.height);
}