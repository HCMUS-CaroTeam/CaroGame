#pragma once
#include "raylib.h"
#include "Control/input_mouse.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"
#include "Control/name_input_data.h"

/*
    ui_name_setup.h

    Màn hình nhập tên người chơi trước khi vào game.

    Làm:
    - Hiển thị ô nhập tên cho Player 1 và Player 2 (hoặc chỉ P1 nếu PvE).
    - Chọn avatar (character-1..4) cho từng người chơi.
    - Nút PLAY → ghi tên vào current().namePlayer1 / namePlayer2 rồi
      chuyển sang SCREEN_PLAY.
    - Nút BACK → quay về SCREEN_SETUP.

    Không làm:
    - Không dùng bất kỳ Button/array nào từ menu_data (tránh conflict).
    - Không thêm field mới vào DataGame (namePlayer1/2 đã có sẵn).

    animIndex dùng: 6, 7, 8  (nằm trong mảng 32 phần tử của ui_button)
*/

void InitNameInputUI();
void ShutdownNameInputUI();

void UpdateNameInputUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
);

void DrawNameInputUI(
    Font fontTitle,
    Font fontSmall,
    const MouseState& mouse,
    const AppSettings& settings
);

void ResetNameInputBuffers();
