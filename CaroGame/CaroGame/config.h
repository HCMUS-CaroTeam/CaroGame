#pragma once
// Cấu hình chung
constexpr int SCREEN_WIDTH = 1400;
constexpr int SCREEN_HEIGHT = 800;
constexpr int TARGET_FPS = 60;

/* 
Đường dẫn đến font chữ được sử dụng trong giao diện
Nếu file font không tồn tại, chương trình sẽ tự động sử dụng font mặc định của raylib
Nếu muốn đổi font chữ thì chỉ cần thay đổi đường dẫn ở đây và đảm bảo file font đó tồn tại trong thư mục assets/fonts/
*/
constexpr const char* FONT_PATH = "assets/fonts/orbitron.ttf";