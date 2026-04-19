#pragma once

#include <unordered_map>
#include <string>
#include "Model/game_data.h"
using namespace std;

// Function save the game state to unorderred_map, key is the name of the save, value is the game data
// This function don't save the game to file, it just save the game to memory, the game will be lost when the program is closed

extern unordered_map<string, DataGame> gameSaves;
inline unordered_map<string, DataGame> &savedGames() {
	return gameSaves;
}
void SaveGame(const DataGame& gameData);

// Function save unordered_map to file, the file name is "saves.dat"
void SaveToFile(const unordered_map<string, DataGame>& gameSaves);