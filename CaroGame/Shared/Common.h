#ifndef COMMON_H
#define COMMON_H

#include "../raylib/include/raylib.h"

// --- CÁC HẰNG SỐ CỦA BÀN CỜ ---
#define BOARD_SIZE 12
#define CELL_SIZE 40
#define OFFSET_X 50
#define OFFSET_Y 50
#define TURN_TIME_LIMIT 15.0f // 15 giây cho Mode Hardcore

// --- CẤU TRÚC DỮ LIỆU ---


// --- BIẾN TOÀN CỤC (Data.cpp sẽ định nghĩa tụi này) ---
extern int _BOARD[BOARD_SIZE][BOARD_SIZE];
extern bool _TURN;           // true (X), false (O)
extern int _GAME_MODE;       // 1: Tiêu chuẩn, 2: Hardcore
extern float _TIME_LEFT;     // Thời gian đếm ngược

#endif