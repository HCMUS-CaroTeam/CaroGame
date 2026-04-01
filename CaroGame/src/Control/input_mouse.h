#pragma once
#include "raylib.h"

/*
    File này dùng để gom phần xử lý chuột cơ bản.

    File này làm:
    - Lưu trạng thái chuột hiện tại.
    - Kiểm tra hover trên hình chữ nhật.
    - Kiểm tra click trên hình chữ nhật.

    File này không làm:
    - Không vẽ giao diện.
    - Không biết đang ở màn hình nào.
    - Không tự xử lý nút cụ thể.

    Cách hoạt động:
    - Mỗi frame gọi GetMouseStateNow().
    - Sau đó truyền MouseState vào các file UI để xử lý hover/click.
*/

// dữ liệu trạng thái chuột hiện tại
struct MouseState
{
    Vector2 position;
    bool leftPressed;
    bool leftDown;
    bool leftReleased;
};

// Lấy trạng thái chuột hiện tại từ raylib.
// Trả về vị trí chuột, vừa nhấn, đang giữ, hay vừa thả chuột trái.
MouseState GetMouseStateNow();

// Kiểm tra chuột có đang nằm trong một hình chữ nhật không.
// Dùng cho hover.
bool IsMouseOverRect(const MouseState& mouse, Rectangle rect);

// Kiểm tra chuột có click vào hình chữ nhật không.
// Điều kiện: chuột đang ở trong rect và chuột trái vừa được nhấn.
bool IsMouseClickedRect(const MouseState& mouse, Rectangle rect);