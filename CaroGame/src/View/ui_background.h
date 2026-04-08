#pragma once
#include "raylib.h"

/*
    Background System

    File này chịu trách nhiệm:
    - Load texture nền + logo
    - Vẽ background cho toàn bộ scene
    - Giải phóng tài nguyên khi thoát

    Nguyên tắc:
    - Không chứa logic game
    - Không phụ thuộc UI cụ thể
    - Chỉ render nền + hiệu ứng nền
*/

// Load toàn bộ asset background (gọi 1 lần hoặc để auto trong draw)
void LoadBackgroundAssets();

// Giải phóng texture
void UnloadBackgroundAssets();

// Chỉ vẽ nền
void DrawBackgroundOnly();

// Chỉ vẽ logo
void DrawLogoOnly();