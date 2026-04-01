#include "input_mouse.h"

/*
    File này cài đặt các hàm chuột cơ bản.

    Cách hoạt động:
    - GetMouseStateNow() đọc dữ liệu chuột từ raylib.
    - IsMouseOverRect() tự kiểm tra chuột có nằm trong vùng rect hay không.
    - IsMouseClickedRect() kết hợp hover + leftPressed để xác định click.
*/

// Đọc dữ liệu chuột ở frame hiện tại từ raylib.
MouseState GetMouseStateNow()
{
    MouseState mouse{};
    mouse.position = GetMousePosition();
    mouse.leftPressed = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
    mouse.leftDown = IsMouseButtonDown(MOUSE_LEFT_BUTTON);
    mouse.leftReleased = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
    return mouse;
}

// Kiểm tra chuột có đang nằm trong vùng hình chữ nhật không.
bool IsMouseOverRect(const MouseState& mouse, Rectangle rect)
{
    return (mouse.position.x >= rect.x &&
        mouse.position.x <= rect.x + rect.width &&
        mouse.position.y >= rect.y &&
        mouse.position.y <= rect.y + rect.height);
}

// Kiểm tra chuột có click vào hình chữ nhật không.
// Đây là hàm tiện lợi để các file UI dùng trực tiếp.
bool IsMouseClickedRect(const MouseState& mouse, Rectangle rect)
{
    return IsMouseOverRect(mouse, rect) && mouse.leftPressed;
}