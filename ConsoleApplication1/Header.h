#pragma once // Chống include nhiều lần
#include <iostream>		
#include <Windows.h> 
#include <conio.h>

using namespace std;

// ------ Hằng số ------
#define BOARD_SIZE 12 
#define LEFT 3 
#define TOP 1 

// ------ Khai báo kiểu dữ liệu ------
struct _POINT { int x, y, c; };

// ------ Khai báo biến toàn cục (Dùng extern) ------
extern _POINT _A[BOARD_SIZE][BOARD_SIZE];
extern bool _TURN;
extern int _COMMAND;
extern int _X, _Y;