#pragma once

#include <unordered_map>
#include <string>
#include "Model/game_data.h"

using namespace std;

extern unordered_map<string, DataGame> gameSaves;
void SaveData(const DataGame& gameData);
void SaveGamesToFile(const unordered_map<string, DataGame>& gameSaves);
void LoadData(const unordered_map<string, DataGame>& gameLoaded, const string& gameName);
void LoadGamesFromFile(unordered_map<string, DataGame>& loadedGames);