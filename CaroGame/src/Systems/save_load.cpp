#include "save_load.h"
#include <ctime>
#include <fstream>
#include <filesystem>

unordered_map<string, DataGame> gameSaves = {};

bool isDuplicateName(const string& name) {
	return gameSaves.count(name) > 0;
}

void ChangeGameName(const char* newName) {
	auto& data = GetCurrentGameData();
	strcpy_s(data.nameGame, sizeof(data.nameGame), newName);
	data.nameGame[sizeof(data.nameGame) - 1] = '\0'; // Đảm bảo kết thúc chuỗi
}

void RenameLoadedGame(const string& oldName, const string& newName) {
	DataGame data = gameSaves[oldName]; // Tìm bản lưu cũ trong map bằng tên cũ
	DeleteGameSave(oldName); // Xóa bản lưu cũ khỏi file và map
	strcpy_s(data.nameGame, sizeof(data.nameGame), newName.c_str()); // Cập nhật tên game trong dữ liệu
	gameSaves[newName] = data; // Thêm bản lưu mới với tên mới vào map
	SaveGamesToFile(gameSaves); // Ghi lại file để đảm bảo thay đổi được lưu	
}

static bool validateGameData(const DataGame& checkedData) {
	// 1. Kiểm tra chuỗi char[] xem có ký tự kết thúc '\0' hợp lệ không
	// Điều này đảm bảo chuỗi không bị tràn khi sử dụng
	bool static_string_ok = false;
	for (int i = 0; i < 50; ++i) {
		if (checkedData.nameGame[i] == '\0') { static_string_ok = true; break; }
	}
	if (!static_string_ok) return false;

	// Tương tự kiểm tra cho namePlayer1 và namePlayer2
	auto checkString = [](const char* str, int size) {
		for (int i = 0; i < size; ++i) {
			if (str[i] == '\0') return true;
		}
		return false;
	};

	if (!checkString(checkedData.namePlayer1, 50)) return false;
	if (!checkString(checkedData.namePlayer2, 50)) return false;

	// 2. Kiểm tra điểm số (không được âm bậy bạ)
	if (checkedData.scorePlayer1 < 0 || checkedData.scorePlayer2 < 0) return false;

	// 3. Kiểm tra mảng trạng thái bàn cờ (board)
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			int val = checkedData.board[i][j];
			// Giả sử chỉ có 3 giá trị: 0 (Trống), 1 (X), 2 (O)
			if (val != CELL_EMPTY && val != CELL_X && val != CELL_O) return false;
		}
	}

	// 4. Kiểm tra lượt đi, kết quả, chế độ chơi (Dựa theo Enum của bạn)
	// Ví dụ: turn chỉ có thể là CELL_X hoặc CELL_O
	if (checkedData.turn != CELL_X && checkedData.turn != CELL_O) return false;

	// 5. Kiểm tra nước đi cuối cùng (Last Move)
	// Hợp lệ là nằm trong bàn cờ, HOẶC bằng -1 (nếu chưa đi nước nào)
	if (checkedData.lastMoveRow != -1 && (checkedData.lastMoveRow < 0 || checkedData.lastMoveRow >= BOARD_SIZE)) return false;
	if (checkedData.lastMoveCol != -1 && (checkedData.lastMoveCol < 0 || checkedData.lastMoveCol >= BOARD_SIZE)) return false;

	// 6. Kiểm tra dữ liệu đường thắng (winLine), số lượng có thể lớn hơn WIN_LENGTH 
	// Do luật chơi, nhưng không được âm hoặc vượt quá kích thước bàn cờ
	if (checkedData.winLineCount < 0 || checkedData.winLineCount > BOARD_SIZE) return false;
	for (int i = 0; i < checkedData.winLineCount; ++i) {
		int r = checkedData.winLine[i][0];
		int c = checkedData.winLine[i][1];
		if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) return false;
	}

	// 7. Kiểm tra thời gian (nếu cần)
	if (checkedData.timeLeft != TIME_LIMIT_NONE && checkedData.timeLeft != TIME_LIMIT_5S && checkedData.timeLeft != TIME_LIMIT_10S && checkedData.timeLeft != TIME_LIMIT_15S) return false;
	if (checkedData.saveTime < 0 || checkedData.saveTime > time(nullptr)) return false; // Thời gian lưu không thể ở tương lai hoặc âm
	// 8. Kiểm tra mức độ khó Bot (nếu chế độ PVE)
	if (checkedData.gameMode == MODE_PVE) {
		if (checkedData.botDifficulty != DIFFICULTY_NONE &&checkedData.botDifficulty != DIFFICULTY_EASY && checkedData.botDifficulty != DIFFICULTY_MEDIUM && checkedData.botDifficulty != DIFFICULTY_HARD) return false;
	}

	// 9. Kiểm tra chế độ PVP (nếu chế độ PVP)
	if (checkedData.gameMode == MODE_PVP) {
		if (checkedData.pvpMode != CLASSIC && checkedData.pvpMode != TOURNAMENT && checkedData.pvpMode != NONE) return false;
	}

	return true; // Tất cả dữ liệu đều hợp lý
}

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
	if (!filesystem::exists("Saves") || !filesystem::is_directory("Saves")) {
		return;
	}

	for (const auto& entry : filesystem::directory_iterator("Saves")) {
		if (entry.is_regular_file() && entry.path().extension() == ".dat") {
			ifstream loadFile(entry.path(), ios::binary);
			if (loadFile) {
				DataGame data = {};

				if (loadFile.read(reinterpret_cast<char*>(&data), sizeof(DataGame))) {
					loadFile.close();

					if (validateGameData(data)) {
						// 1. Lấy tên file thực tế làm Key (bỏ đuôi .dat)
						// Ví dụ: "Saves/Match 1.dat" -> "Match 1"
						string fileNameKey = entry.path().stem().string();

						// 2. PHÒNG VỆ: Đồng bộ lại data.nameGame theo tên file mới nếu bị đổi ngoài win
						// Tránh trường hợp tên file bên ngoài và tên ván cờ bên trong lệch nhau
						memset(data.nameGame, 0, sizeof(data.nameGame));
						strcpy_s(data.nameGame, sizeof(data.nameGame), fileNameKey.c_str());

						// 3. Đưa vào map bằng std::move để tối ưu hiệu năng
						loadedGames[fileNameKey] = data;
					}
				}
				else {
					loadFile.close();
				}
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