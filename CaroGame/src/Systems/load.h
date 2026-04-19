#pragma once
#include <unordered_map>
#include <string>
#include "Model/game_data.h"

using namespace std;

// Function load the game state from unorderred_map, key is the name of the save, value is the game data
//unordered_map<string, DataGame> 
extern unordered_map<string, DataGame> gameLoads;
inline unordered_map<string, DataGame>& loadedGames() {
	return gameLoads;
}
unordered_map<string, DataGame> LoadGamesFromFile();
// This function don't load the game from file, it just load the game from unordered_map, the game will be lost when the program is closed
// It helps to load the game when player have game name, if the game name is not exist, it will return an empty game data
void LoadGame(const string& gameName, const unordered_map<string, DataGame>& gameLoaded);