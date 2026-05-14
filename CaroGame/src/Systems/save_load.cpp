#include "save_load.h"
#include <ctime>
#include <fstream>
#include <filesystem>

unordered_map<string, DataGame> gameSaves = {};

void SaveData(DataGame& gameData) {
	gameData.saveTime = time(nullptr); // Cập nhật thời gian lưu hiện tại
	// Use the name of the game as the key for saving
	gameSaves[string(gameData.nameGame)] = gameData;
}

void SaveGamesToFile(const unordered_map<string, DataGame>& gameSaves) {
	for (const auto& [name, data] : gameSaves) {
		string filename = "Saves/" + name + ".dat"; // Create a separate file for each save
		ofstream saveFile(filename, ios::binary | ios::trunc);
		if (saveFile) {
			saveFile.write(reinterpret_cast<const char*>(&data), sizeof(DataGame));
			saveFile.close();
		}
	}
}

void LoadData(const unordered_map<string, DataGame>& gameLoaded, const string& gameName) {
	auto it = gameLoaded.find(gameName);
	if (it != gameLoaded.end()) {
		DataGame loadedData = it->second;
		GetCurrentGameData() = loadedData; // Update current game data with loaded data
	}
	// If game name not found, do nothing or handle as needed
}

void LoadGamesFromFile(unordered_map<string, DataGame>& loadedGames) {
	for (const auto& entry : filesystem::directory_iterator("Saves")) {
		if (entry.is_regular_file() && entry.path().extension() == ".dat") {
			ifstream loadFile(entry.path(), ios::binary);
			if (loadFile) {
				DataGame data = {};
				loadFile.read(reinterpret_cast<char*>(&data), sizeof(DataGame));
				loadFile.close();
				loadedGames[string(data.nameGame)] = data; // Use game name as key
			}
		}
	}
}

void DeleteGameSave(const string& gameName) {
	string filename = "Saves/" + gameName + ".dat";
	filesystem::remove(filename); // Remove the save file from the filesystem
	gameSaves.erase(gameName); // Remove the entry from the in-memory map
}
	
void CreateFolder() {
	if (!filesystem::exists("Saves")) {
		filesystem::create_directory("Saves");
	}
}