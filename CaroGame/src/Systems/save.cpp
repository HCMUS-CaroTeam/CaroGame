#include "save.h"
#include <fstream>

static unordered_map<string, DataGame> gameSaves = {};

void SaveGame(const DataGame& gameData) {
	// Use the name of the game as the key for saving
	gameSaves[string(gameData.nameGame)] = gameData;
}

void SaveToFile(const unordered_map<string, DataGame>& gameSaves) {
	ofstream outFile("saves.dat", ios::binary);
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