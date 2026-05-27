#pragma once

#include <unordered_map>
#include <string>
#include "Model/game_data.h"

using namespace std;

extern unordered_map<string, DataGame> gameSaves;
bool isDuplicateName(const string& name);
void SaveData(DataGame& gameData);
void SaveGamesToFile(const unordered_map<string, DataGame>& gameSaves);
void LoadData(const unordered_map<string, DataGame>& gameLoaded, const string& gameName);
void LoadGamesFromFile(unordered_map<string, DataGame>& loadedGames);
void DeleteGameSave(const string& gameName);
void CreateFolder();

// Hàm thay đổi tên game trong currentGame, dùng khi load game hoặc đổi tên save
extern void ChangeGameName(const char* newName);
// Hàm này được gọi sau khi load game để đổi tên game trong unordered_map gameSaves
// Đảm bảo nhất quán tên game trong currentGame và tên key trong gameSaves khi người chơi đổi tên save sau khi load game
extern void RenameLoadedGame(const string& oldName, const string& newName);