#include "Systems/audio_manager.h"
#include "Model/game_data.h"
#include <string>

namespace
{
constexpr int MUSIC_NONE = 0;
constexpr int MUSIC_REGULAR = 1;
constexpr int MUSIC_BATTLE = 2;

std::string FindAssetPath(const char* relativePath)
{
    const char* bases[] = {
        "",
        "CaroGame/",
        "../CaroGame/",
        "../../CaroGame/"
    };

    for (const char* base : bases)
    {
        std::string path = std::string(base) + relativePath;
        if (FileExists(path.c_str()))
        {
            return path;
        }
    }

    return "";
}

Music LoadMusicAsset(const char* lowerPath, const char* upperPath, bool& loaded)
{
    std::string path = FindAssetPath(lowerPath);
    if (path.empty() && upperPath)
    {
        path = FindAssetPath(upperPath);
    }

    if (!path.empty())
    {
        loaded = true;
        TraceLog(LOG_INFO, "AUDIO: Load music %s", path.c_str());
        return LoadMusicStream(path.c_str());
    }

    loaded = false;
    TraceLog(LOG_WARNING, "AUDIO: Missing music %s", lowerPath);
    return Music{};
}

Sound LoadSoundAsset(const char* lowerPath, const char* upperPath, bool& loaded)
{
    std::string path = FindAssetPath(lowerPath);
    if (path.empty() && upperPath)
    {
        path = FindAssetPath(upperPath);
    }

    if (!path.empty())
    {
        loaded = true;
        TraceLog(LOG_INFO, "AUDIO: Load sound %s", path.c_str());
        return LoadSound(path.c_str());
    }

    loaded = false;
    TraceLog(LOG_WARNING, "AUDIO: Missing sound %s", lowerPath);
    return Sound{};
}

Music* ActiveMusic(AudioAssets& audio)
{
    if (audio.activeMusic == MUSIC_BATTLE && audio.battleMusicLoaded) return &audio.battleMusic;
    if (audio.activeMusic == MUSIC_REGULAR && audio.regularMusicLoaded) return &audio.regularMusic;
    return nullptr;
}

void PlayMusicByType(AudioAssets& audio, int musicType)
{
    if (audio.activeMusic == musicType) return;

    Music* current = ActiveMusic(audio);
    if (current) StopMusicStream(*current);

    audio.activeMusic = musicType;

    Music* next = ActiveMusic(audio);
    if (next) PlayMusicStream(*next);
}
}

void InitGameAudio(AudioAssets& audio)
{
    InitAudioDevice();

    audio.regularMusic = LoadMusicAsset(
        "assets/audio/BackGround/BG_(Regular).wav",
        "Assets/audio/BackGround/BG_(Regular).wav",
        audio.regularMusicLoaded);
    audio.battleMusic = LoadMusicAsset(
        "assets/audio/BackGround/BG_(Battle).wav",
        "Assets/audio/BackGround/BG_(Battle).wav",
        audio.battleMusicLoaded);

    audio.clickSound = LoadSoundAsset(
        "assets/audio/SFX/CLICK/click_bone_crack.wav",
        "Assets/audio/SFX/CLICK/click_bone_crack.wav",
        audio.clickLoaded);
    audio.hoverSound = LoadSoundAsset(
        "assets/audio/SFX/HOVER/hover_heartbeat.mp3",
        "Assets/audio/SFX/HOVER/hover_heartbeat.mp3",
        audio.hoverLoaded);
    audio.pieceSound = LoadSoundAsset(
        "assets/audio/SFX/DANH_X_O/click_004.ogg",
        "Assets/audio/SFX/DANH_X_O/click_004.ogg",
        audio.pieceLoaded);
    audio.winSound = LoadSoundAsset(
        "assets/audio/SFX/WIN/win_sound.wav",
        "Assets/audio/SFX/WIN/win_sound.wav",
        audio.winLoaded);
    audio.loseSound = LoadSoundAsset(
        "assets/audio/SFX/LOSE/lightyeartraxx-kl-peach-game-over-iii-142453.mp3",
        "Assets/audio/SFX/LOSE/lightyeartraxx-kl-peach-game-over-iii-142453.mp3",
        audio.loseLoaded);

    if (audio.regularMusicLoaded)
    {
        audio.activeMusic = MUSIC_REGULAR;
        PlayMusicStream(audio.regularMusic);
    }
    else if (audio.battleMusicLoaded)
    {
        audio.activeMusic = MUSIC_BATTLE;
        PlayMusicStream(audio.battleMusic);
    }
}

void UpdateGameAudio(AudioAssets& audio, const AppSettings& settings)
{
    Music* music = ActiveMusic(audio);
    if (music)
    {
        UpdateMusicStream(*music);
        SetMusicVolume(*music, settings.musicVolume);
    }

    if (audio.clickLoaded) SetSoundVolume(audio.clickSound, settings.sfxVolume);
    if (audio.hoverLoaded) SetSoundVolume(audio.hoverSound, settings.sfxVolume);
    if (audio.pieceLoaded) SetSoundVolume(audio.pieceSound, settings.sfxVolume);
    if (audio.winLoaded) SetSoundVolume(audio.winSound, settings.sfxVolume);
    if (audio.loseLoaded) SetSoundVolume(audio.loseSound, settings.sfxVolume);
}

void ShutdownGameAudio(AudioAssets& audio)
{
    Music* music = ActiveMusic(audio);
    if (music) StopMusicStream(*music);

    if (audio.regularMusicLoaded)
    {
        UnloadMusicStream(audio.regularMusic);
        audio.regularMusicLoaded = false;
    }

    if (audio.battleMusicLoaded)
    {
        UnloadMusicStream(audio.battleMusic);
        audio.battleMusicLoaded = false;
    }

    if (audio.clickLoaded)
    {
        UnloadSound(audio.clickSound);
        audio.clickLoaded = false;
    }

    if (audio.hoverLoaded)
    {
        UnloadSound(audio.hoverSound);
        audio.hoverLoaded = false;
    }

    if (audio.pieceLoaded)
    {
        UnloadSound(audio.pieceSound);
        audio.pieceLoaded = false;
    }

    if (audio.winLoaded)
    {
        UnloadSound(audio.winSound);
        audio.winLoaded = false;
    }

    if (audio.loseLoaded)
    {
        UnloadSound(audio.loseSound);
        audio.loseLoaded = false;
    }

    audio.activeMusic = MUSIC_NONE;
    CloseAudioDevice();
}

void SetBattleMusic(AudioAssets& audio, bool battle)
{
    if (battle && audio.battleMusicLoaded)
    {
        PlayMusicByType(audio, MUSIC_BATTLE);
        return;
    }

    if (audio.regularMusicLoaded)
    {
        PlayMusicByType(audio, MUSIC_REGULAR);
    }
}

void PlayMenuClick(AudioAssets& audio, const AppSettings& settings)
{
    if (!audio.clickLoaded) return;

    SetSoundVolume(audio.clickSound, settings.sfxVolume);
    StopSound(audio.clickSound);
    PlaySound(audio.clickSound);
}

void PlayMenuHover(AudioAssets& audio)
{
    if (!audio.hoverLoaded) return;

    StopSound(audio.hoverSound);
    PlaySound(audio.hoverSound);
}

void PlayPieceSound(AudioAssets& audio, const AppSettings& settings)
{
    if (!audio.pieceLoaded) return;

    SetSoundVolume(audio.pieceSound, settings.sfxVolume);
    StopSound(audio.pieceSound);
    PlaySound(audio.pieceSound);
}

void PlayGameResultSound(AudioAssets& audio, int result, int gameMode, const AppSettings& settings)
{
    if (result == RESULT_DRAW || result == RESULT_ONGOING) return;

    const bool playerLostToBot = (gameMode == MODE_PVE && result == RESULT_O_WINS);
    Sound* resultSound = playerLostToBot ? &audio.loseSound : &audio.winSound;
    bool loaded = playerLostToBot ? audio.loseLoaded : audio.winLoaded;
    if (!loaded) return;

    SetSoundVolume(*resultSound, settings.sfxVolume);
    StopSound(*resultSound);
    PlaySound(*resultSound);
}
