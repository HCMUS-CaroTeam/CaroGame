#pragma once
#include <iostream>
#include <Windows.h>
#include <conio.h>

using namespace std;

// ------ Hằng số ------
#define BOARD_SIZE 12
#define LEFT 3
#define TOP 1

// ------ Khai báo biến toàn cục (Dùng extern) ------
extern char _BOARD[BOARD_SIZE][BOARD_SIZE]; // Mảng này dùng để lưu trạng thái bàn cờ khi cần thiết (ví dụ: lưu vào file)
extern bool _TURN;
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
#pragma pack(pop)
