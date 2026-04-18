#pragma once
#include <fstream>
#include <sstream>
#include <string>

struct AppSettings
{
    bool  backgroundGrid  = true;
    float uiBrightness    = 0.70f;
    float musicVolume     = 0.60f;
    float sfxVolume       = 0.75f;
    bool  highlightLastMove = true;
    bool  showCoordinates   = false;
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

        if      (key == "uiBrightness")      val >> s.uiBrightness;
        else if (key == "musicVolume")       val >> s.musicVolume;
        else if (key == "sfxVolume")         val >> s.sfxVolume;
        else if (key == "backgroundGrid")    { int v; val >> v; s.backgroundGrid    = v != 0; }
        else if (key == "highlightLastMove") { int v; val >> v; s.highlightLastMove = v != 0; }
        else if (key == "showCoordinates")   { int v; val >> v; s.showCoordinates   = v != 0; }
    }
}
