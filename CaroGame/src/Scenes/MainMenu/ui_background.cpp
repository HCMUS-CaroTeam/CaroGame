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

static void DrawBackgroundFallback()
{
    ClearBackground(Color{ 10, 10, 28, 255 });

    // vài lớp màu nhẹ để nền đỡ trống nếu thiếu texture
    DrawCircleGradient(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 3,
        380.0f,
        Color{ 40, 40, 90, 90 },
        Color{ 0, 0, 0, 0 }
    );

    DrawCircleGradient(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT,
        600.0f,
        Color{ 20, 20, 50, 120 },
        Color{ 0, 0, 0, 0 }
    );
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

void DrawBackgroundScene()
{
    LoadBackgroundAssets();

    // 1) nền
    if (IsTextureReadyEx(texBackground))
    {
        ClearBackground(BLACK);
        DrawScaledFullScreen(texBackground);
    }
    else
    {
        DrawBackgroundFallback();
    }

    // 2) overlay tối nhẹ để UI nổi hơn
    DrawRectangle(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        Color{ 8, 6, 20, 55 }
    );

    // 3) glow nhẹ phía trên trung tâm
    DrawCircleGradient(
        SCREEN_WIDTH / 2,
        160,
        260.0f,
        Color{ 120, 120, 200, 28 },
        Color{ 0, 0, 0, 0 }
    );

    // 4) logo
    if (IsTextureReadyEx(texLogo))
    {
        const float time = GetTime();
        const float floatOffsetY = std::sin(time * 1.8f) * 6.0f;

        const float targetWidth = 900.0f;
        const float scale = targetWidth / static_cast<float>(texLogo.width);
        const float targetHeight = static_cast<float>(texLogo.height) * scale;

        Rectangle dest{
            SCREEN_WIDTH * 0.5f - targetWidth * 0.5f,
            35.0f + floatOffsetY,
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