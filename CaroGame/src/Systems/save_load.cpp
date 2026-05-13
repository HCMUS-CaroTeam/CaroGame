#include "save_load.h"
#include <ctime>
#include <fstream>

unordered_map<string, DataGame> gameSaves = {};

void SaveData(DataGame& gameData) {
	gameData.saveTime = time(nullptr); // Cập nhật thời gian lưu hiện tại
	// Use the name of the game as the key for saving
	gameSaves[string(gameData.nameGame)] = gameData;
}

void SaveGamesToFile(const unordered_map<string, DataGame>& gameSaves) {
	ofstream outFile("saves.dat", ios::binary | ios::trunc);
	if (!outFile) {
		return;
	}
	size_t numSaves = gameSaves.size();
	outFile.write(reinterpret_cast<const char*>(&numSaves), sizeof(numSaves));
	for (const auto& pair : gameSaves) {
		const DataGame& data = pair.second;
		outFile.write(reinterpret_cast<const char*>(&data), sizeof(DataGame));
	}
	outFile.close();
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
	ifstream inFile("saves.dat", ios::binary); //Maybe change filename to save.
	if (!inFile) {
		return; // Return if file cannot be opened
	}
	size_t numSaves = 0;
	inFile.read(reinterpret_cast<char*>(&numSaves), sizeof(numSaves));
	for (size_t i = 0; i < numSaves; ++i) {
		DataGame data = {};
		inFile.read(reinterpret_cast<char*>(&data), sizeof(DataGame));
		loadedGames[string(data.nameGame)] = data; // Use game name as key
	}
	inFile.close();
	gameSaves = loadedGames; // Update global saves with loaded games
}