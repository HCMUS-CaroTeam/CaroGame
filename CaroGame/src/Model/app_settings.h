#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include  "game_data.h"

struct AppSettings
{
    bool  backgroundGrid  = true;
    float uiBrightness    = 0.70f;
    float musicVolume     = 0.60f;
    float sfxVolume       = 0.75f;
    bool  highlightLastMove = true;
    bool  showCoordinates   = false;

    // --- NHÓM SETUP GAME (Đã nâng cấp lên Enum) ---
    GameMode      gameMode = MODE_PVP;
    BotDifficulty botDifficulty = DIFFICULTY_EASY;
    PVPMode       pvpMode = CLASSIC;
    TimeLimit     timeLeft = TIME_LIMIT_NONE; 
    int           boardSize = 15;
};

inline void SaveSettings(const AppSettings& s)
{
    std::ofstream f("settings.cfg");
    if (!f) return;

    f << "backgroundGrid="    << s.backgroundGrid     << "\n";
    f << "uiBrightness="      << s.uiBrightness       << "\n";
    f << "musicVolume="       << s.musicVolume        << "\n";
    f << "sfxVolume="         << s.sfxVolume          << "\n";
    f << "highlightLastMove=" << s.highlightLastMove  << "\n";
    f << "showCoordinates="   << s.showCoordinates    << "\n";

    // Ép kiểu Enum về số nguyên (int) để lưu xuống file
    f << "gameMode=" << static_cast<int>(s.gameMode) << "\n";
    f << "botDifficulty=" << static_cast<int>(s.botDifficulty) << "\n";
    f << "pvpMode=" << static_cast<int>(s.pvpMode) << "\n";
    f << "timeLeft=" << static_cast<int>(s.timeLeft) << "\n";
    f << "boardSize=" << s.boardSize << "\n";
}

inline void LoadSettings(AppSettings& s)
{
    std::ifstream f("settings.cfg");
    if (!f) return;

    std::string line;
    while (std::getline(f, line))
    {
        const auto sep = line.find('=');
        if (sep == std::string::npos) continue;

        const std::string key = line.substr(0, sep);
        std::istringstream val(line.substr(sep + 1));
		int v; // Biến tạm để đọc số nguyên trước khi ép kiểu về Enum

        if      (key == "uiBrightness")      val >> s.uiBrightness;
        else if (key == "musicVolume")       val >> s.musicVolume;
        else if (key == "sfxVolume")         val >> s.sfxVolume;
        else if (key == "backgroundGrid")    { int v; val >> v; s.backgroundGrid    = v != 0; }
        else if (key == "highlightLastMove") { int v; val >> v; s.highlightLastMove = v != 0; }
        else if (key == "showCoordinates")   { int v; val >> v; s.showCoordinates   = v != 0; }

        // Đọc số nguyên từ file, sau đó ép kiểu ngược lại về Enum
        else if (key == "gameMode") { val >> v; s.gameMode = static_cast<GameMode>(v); }
        else if (key == "botDifficulty") { val >> v; s.botDifficulty = static_cast<BotDifficulty>(v); }
        else if (key == "pvpMode") { val >> v; s.pvpMode = static_cast<PVPMode>(v); }
        else if (key == "timeLeft") { val >> v; s.timeLeft = static_cast<TimeLimit>(v); }
        else if (key == "boardSize")     val >> s.boardSize;
    }
}
