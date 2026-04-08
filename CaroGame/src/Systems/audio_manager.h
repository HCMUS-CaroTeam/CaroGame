#pragma once
#include "raylib.h"
#include "Model/app_settings.h"

/*
    File này khai báo phần quản lý âm thanh.

    File này làm:
    - Lưu các tài nguyên âm thanh.
    - Khai báo hàm khởi tạo, cập nhật, giải phóng âm thanh.
    - Khai báo hàm phát tiếng click và hover.

    Ý nghĩa:
    - Các file UI không cần tự load hay quản lý sound.
    - Chỉ cần gọi PlayMenuClick() hoặc PlayMenuHover().
*/

// Gom toàn bộ tài nguyên âm thanh của chương trình vào một struct.
// Các cờ *Loaded dùng để biết file đó có load thành công hay chưa.
struct AudioAssets
{
    Music bgMusic{};
    Sound clickSound{};
    Sound hoverSound{};

    bool musicLoaded = false;
    bool clickLoaded = false;
    bool hoverLoaded = false;
};

void InitGameAudio(AudioAssets& audio);
void UpdateGameAudio(AudioAssets& audio, const AppSettings& settings);
void ShutdownGameAudio(AudioAssets& audio);

void PlayMenuClick(AudioAssets& audio, const AppSettings& settings);
void PlayMenuHover(AudioAssets& audio);