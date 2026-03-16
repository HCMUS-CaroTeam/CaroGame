#pragma once
#include "Common.h"

void FixConsoleWindow();
void GotoXY(int x, int y);
void DrawBoard(int pSize);
int ProcessFinish(int pWhoWin);
int AskContinue(int pWhoWin);
void DrawMainMenu(int selectedOption);
void DrawPlayerStats();
void DrawPieces();
