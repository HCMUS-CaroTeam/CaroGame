#ifndef COMMON_H
#define COMMON_H

// --- THƯ VIỆN BỔ SUNG TỪ NHÁNH MASTER ---
#include <iostream>
#include <conio.h>

// Bỏ using namespace std ở đây để không làm hỏng thư viện hệ thống

// --- CÁC HẰNG SỐ CỦA BÀN CỜ ---
#define BOARD_SIZE 12  // Kích thước bàn cờ (12x12)
#define CELL_SIZE 40   // Kích thước mỗi ô vuông (40x40 pixel)
#define OFFSET_X 100   // Khoảng cách từ mép trái màn hình đến bàn cờ (pixel)
#define OFFSET_Y 100   // Khoảng cách từ mép trên màn hình đến bàn cờ (pixel)


// --- BIẾN TOÀN CỤC CHUNG ---
extern char _BOARD[BOARD_SIZE][BOARD_SIZE]; // Mảng trạng thái bàn cờ (Thay thế _A)
extern bool _TURN;                          // Lượt đi: true (X), false (O)
extern int _COMMAND;
extern int _X, _Y;
extern int _COL, _ROW;
extern short XScore;
extern short OScore;
extern short NumberOfRounds;

// ------ Cấu trúc lưu trữ tiến trình game (dùng để lưu vào file) ------
#pragma pack(push, 1)
struct Progress
{
    // Thông tin về tên ván chơi (người chơi đặt) và những người chơi
    char gameName[50];
    char playerXName[50];
    char playerOName[50];

    // Thông tin trận đấu
    short playerXScore;
    short playerOScore;
    short numberOfRounds;

    // Trạng thái bàn cờ
    char boardState[BOARD_SIZE][BOARD_SIZE];
    bool isXTurn;
};
#pragma pack(pop) // <--- QUAN TRỌNG: Phải có cái này để trả lại bộ nhớ bình thường

#endif#endif