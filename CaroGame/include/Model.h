#pragma once
#include "Common.h"
#include <vector>

void ResetData(bool = false);
void SaveGameProgress(const char *filename);
std::vector<Progress> LoadGameProgress(const char* filename);
void DrawHistoryMenu(const std::vector<Progress>&, int );
int TestBoard();
int CheckBoard();