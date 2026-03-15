#pragma once
#include "Common.h"
#include <vector>

void ResetData(bool = false);
void SaveGameProgress(const char *filename);
void LoadGameProgress(const char *filename, vector<Progress> &);
void PrintGameHistory();
void GarbageCollect();
int TestBoard();
int CheckBoard();