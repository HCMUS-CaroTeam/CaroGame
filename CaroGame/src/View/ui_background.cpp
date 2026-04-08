#include "ui_background.h"
#include "Model/config.h"
#include <cmath>
using namespace std;

static Texture2D texBackground{};
static Texture2D texLogo{};
static bool gBackgroundLoaded = false;

static bool IsTextureReadyEx(const Texture2D& tex)
{
    return tex.id != 0;
}

static Texture2D LoadTextureFromCandidates(const char* a, const char* b = nullptr)
{
    if (a && FileExists(a)) return LoadTexture(a);
    if (b && FileExists(b)) return LoadTexture(b);
    return Texture2D{};
}

void LoadBackgroundAssets()
{
    if (gBackgroundLoaded) return;

    // Background chính
    texBackground = LoadTextureFromCandidates(
        "assets/bg/background.png",
        "background.png"
    );

    // Logo game
    texLogo = LoadTextureFromCandidates(
        "assets/bg/game name.png",
        "game name.png"
    );

    gBackgroundLoaded = true;
}

void UnloadBackgroundAssets()
{
    if (IsTextureReadyEx(texBackground))
    {
        UnloadTexture(texBackground);
        texBackground = {};
    }

    if (IsTextureReadyEx(texLogo))
    {
        UnloadTexture(texLogo);
        texLogo = {};
    }

    gBackgroundLoaded = false;
}

static void DrawScaledFullScreen(const Texture2D& tex)
{
    DrawTexturePro(
        tex,
        Rectangle{ 0.0f, 0.0f, static_cast<float>(tex.width), static_cast<float>(tex.height) },
        Rectangle{ 0.0f, 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

void DrawBackgroundOnly()
{
    LoadBackgroundAssets();
    ClearBackground(BLACK);
    DrawScaledFullScreen(texBackground);


    DrawRectangle(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        Color{ 8, 6, 20, 55 }
    );

    DrawCircleGradient(
        SCREEN_WIDTH / 2,
        160,
        260.0f,
        Color{ 120, 120, 200, 28 },
        Color{ 0, 0, 0, 0 }
    );
}

void DrawLogoOnly()
{
    LoadBackgroundAssets();

    if (IsTextureReadyEx(texLogo))
    {
        const float time = GetTime();
        const float floatOffsetY = sin(time * 1.8f) * 6.0f;

        const float targetWidth = SCREEN_WIDTH * 1.0f;
        const float scale = targetWidth / static_cast<float>(texLogo.width);
        const float targetHeight = static_cast<float>(texLogo.height) * scale;

        Rectangle dest{
            SCREEN_WIDTH * 0.5f - targetWidth * 0.5f,
            120.0f + floatOffsetY,
            targetWidth,
            targetHeight
        };

        DrawTexturePro(
            texLogo,
            Rectangle{ 0.0f, 0.0f, static_cast<float>(texLogo.width), static_cast<float>(texLogo.height) },
            dest,
            Vector2{ 0.0f, 0.0f },
            0.0f,
            WHITE
        );
    }
}