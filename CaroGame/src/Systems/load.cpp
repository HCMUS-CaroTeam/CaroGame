#include <fstream>
#include "load.h"

static unordered_map<string, DataGame> gameLoads = {};

unordered_map<string, DataGame> LoadGamesFromFile() {
	unordered_map<string, DataGame> loadedGames;
	ifstream inFile("saves.dat", ios::binary | ios::trunc); //Maybe change filename to save.
	if (!inFile) {
		return loadedGames; // Return empty if file cannot be opened
	}
	size_t numSaves = 0;
	inFile.read(reinterpret_cast<char*>(&numSaves), sizeof(numSaves));
	for (size_t i = 0; i < numSaves; ++i) {
		DataGame data = {};
		inFile.read(reinterpret_cast<char*>(&data), sizeof(DataGame));
		loadedGames[string(data.nameGame)] = data; // Use game name as key
	}
	inFile.close();
	return loadedGames;
}

void LoadGame(const string& gameName, const unordered_map<string, DataGame>& gameLoaded) {
	auto it = gameLoaded.find(gameName);
	if (it != gameLoaded.end()) {
		DataGame loadedData = it->second;
		GetCurrentGameData() = loadedData; // Update current game data with loaded data
	}
	// If game name not found, do nothing or handle as needed
}