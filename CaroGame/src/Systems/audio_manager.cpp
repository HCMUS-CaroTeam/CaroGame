#include "Systems/audio_manager.h"

void InitGameAudio(AudioAssets& audio)
{
    InitAudioDevice();

    if (FileExists("assets/audio/menu_music.mp3"))
    {
        audio.bgMusic = LoadMusicStream("assets/audio/menu_music.mp3");
        audio.musicLoaded = true;
        PlayMusicStream(audio.bgMusic);
    }

    if (FileExists("assets/audio/menu_click.ogg"))
    {
        audio.clickSound = LoadSound("assets/audio/menu_click.ogg");
        audio.clickLoaded = true;
    }

    if (FileExists("assets/audio/menu_hover.ogg"))
    {
        audio.hoverSound = LoadSound("assets/audio/menu_hover.ogg");
        audio.hoverLoaded = true;
    }
}

void UpdateGameAudio(AudioAssets& audio, const AppSettings& settings)
{
    if (audio.musicLoaded)
    {
        UpdateMusicStream(audio.bgMusic);
        SetMusicVolume(audio.bgMusic, settings.musicVolume);
    }

    if (audio.clickLoaded)
    {
        SetSoundVolume(audio.clickSound, settings.sfxVolume);
    }

    if (audio.hoverLoaded)
    {
        SetSoundVolume(audio.hoverSound, settings.sfxVolume);
    }
}

void ShutdownGameAudio(AudioAssets& audio)
{
    if (audio.musicLoaded)
    {
        StopMusicStream(audio.bgMusic);
        UnloadMusicStream(audio.bgMusic);
        audio.musicLoaded = false;
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

    CloseAudioDevice();
}

void PlayMenuClick(AudioAssets& audio, const AppSettings& settings)
{
    (void)settings;
    if (!audio.clickLoaded) return;

    StopSound(audio.clickSound);
    PlaySound(audio.clickSound);
}

void PlayMenuHover(AudioAssets& audio)
{
    if (!audio.hoverLoaded) return;

    if (!IsSoundPlaying(audio.hoverSound))
    {
        PlaySound(audio.hoverSound);
    }
}