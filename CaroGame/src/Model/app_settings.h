#pragma once

struct AppSettings
{
    float neonGlow = 0.80f;
    bool gridAnimation = true;
    bool backgroundGrid = true;
    float uiBrightness = 0.70f;

    float musicVolume = 0.60f;
    float sfxVolume = 0.75f;
    bool menuClickSound = true;

    bool highlightLastMove = true;
    bool winLineAnimation = true;
    bool showCoordinates = false;

	int gameMode = 1; // 1: PvP, 2: PvE
	int botDifficulty = 1; // 1: Easy, 2: Medium, 3: Hard
	int pvpMode = 1; // 1: Classic, 2: Tournament
};