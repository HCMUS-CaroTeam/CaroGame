#include "View/ui_button.h"
#include "Model/config.h"
#include <algorithm>

using namespace std;

static Texture2D gButtonIdle{};
static Texture2D gButtonPressed{};
static bool gButtonIdleLoaded = false;
static bool gButtonPressedLoaded = false;
static bool gButtonIdleAttempted = false;
static bool gButtonPressedAttempted = false;

static bool  gWasHover[32] = {};
static float gHoverAnim[32] = {};
static float gPressAnim[32] = {};

// Vùng pixel thật của nút trong ảnh gốc 576x324
static constexpr Rectangle SRC_IDLE = { 211.0f, 180.0f, 77.0f, 33.0f };
static constexpr Rectangle SRC_PRESSED = { 319.0f, 178.0f, 75.0f, 35.0f };

// Kích thước hiển thị mặc định
static constexpr float BTN_W = 280.0f;
static constexpr float BTN_H = 80.0f;

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

static void LoadButtonAssets()
{
    if (!gButtonIdleAttempted)
    {
        gButtonIdleAttempted = true;
        gButtonIdle = LoadTextureFromCandidates("assets/bg/button1.png", "button1.png");
        gButtonIdleLoaded = IsTextureReadyEx(gButtonIdle);

        if (!gButtonIdleLoaded)
        {
            TraceLog(LOG_WARNING, "BUTTON: Khong load duoc button1.png");
        }
    }

    if (!gButtonPressedAttempted)
    {
        gButtonPressedAttempted = true;
        gButtonPressed = LoadTextureFromCandidates("assets/bg/button2.png", "button2.png");
        gButtonPressedLoaded = IsTextureReadyEx(gButtonPressed);

        if (!gButtonPressedLoaded)
        {
            TraceLog(LOG_WARNING, "BUTTON: Khong load duoc button2.png");
        }
    }
}

static Rectangle GetDrawButtonRect(const Button& button, bool hovered, bool pressed, float hoverAnim)
{
    Rectangle rect = GetButtonRect(button);

    if (hovered && !pressed)
    {
        const float growX = 10.0f * hoverAnim;
        const float growY = 6.0f * hoverAnim;

        rect.x -= growX * 0.5f;
        rect.y -= growY * 0.5f;
        rect.width += growX;
        rect.height += growY;
    }

    if (pressed)
    {
        rect.y += 3.0f;
    }

    return rect;
}

static void DrawButtonTextureCropped(const Texture2D& tex, Rectangle src, Rectangle dest)
{
    DrawTexturePro(
        tex,
        src,
        dest,
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

static void UpdateButtonAnim(int index, bool hovered, bool pressed, float dt)
{
    const float hoverTarget = hovered ? 1.0f : 0.0f;
    const float pressTarget = pressed ? 1.0f : 0.0f;

    if (gHoverAnim[index] < hoverTarget)
    {
        gHoverAnim[index] = min(hoverTarget, gHoverAnim[index] + dt * 8.0f);
    }
    else
    {
        gHoverAnim[index] = max(hoverTarget, gHoverAnim[index] - dt * 8.0f);
    }

    if (gPressAnim[index] < pressTarget)
    {
        gPressAnim[index] = min(pressTarget, gPressAnim[index] + dt * 16.0f);
    }
    else
    {
        gPressAnim[index] = max(pressTarget, gPressAnim[index] - dt * 16.0f);
    }
}

void InitUIButtonSystem()
{
    LoadButtonAssets();

    for (int i = 0; i < 32; ++i)
    {
        gWasHover[i] = false;
        gHoverAnim[i] = 0.0f;
        gPressAnim[i] = 0.0f;
    }
}

void ShutdownUIButtonSystem()
{
    if (gButtonIdleLoaded)
    {
        UnloadTexture(gButtonIdle);
        gButtonIdle = {};
        gButtonIdleLoaded = false;
    }

    if (gButtonPressedLoaded)
    {
        UnloadTexture(gButtonPressed);
        gButtonPressed = {};
        gButtonPressedLoaded = false;
    }

    gButtonIdleAttempted = false;
    gButtonPressedAttempted = false;
}

Rectangle GetButtonRect(const Button& button)
{
    return Rectangle{ button.position.x, button.position.y, BTN_W, BTN_H };
}

void UpdateUIButton(
    int index,
    const Button& button,
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    bool& hovered,
    bool& pressed
)
{
    const Rectangle hitRect = GetButtonRect(button);
    hovered = IsMouseOverRect(mouse, hitRect);
    pressed = hovered && mouse.leftDown;

    if (hovered && !gWasHover[index])
    {
        PlayMenuHover(audio);
    }

    UpdateButtonAnim(index, hovered, pressed, dt);
    gWasHover[index] = hovered;
}

void DrawUIButton(
    int index,
    const Button& button,
    Font fontTitle,
    bool hovered,
    bool pressed
)
{
    const Rectangle drawRect = GetDrawButtonRect(button, hovered, pressed, gHoverAnim[index]);

    if (pressed && gButtonPressedLoaded)
    {
        DrawButtonTextureCropped(gButtonPressed, SRC_PRESSED, drawRect);
    }
    else if (gButtonIdleLoaded)
    {
        DrawButtonTextureCropped(gButtonIdle, SRC_IDLE, drawRect);
    }

    const char* text = button.title.c_str();
    const float fontSize = 28.0f;
    const float spacing = 2.0f;

    Vector2 textSize = MeasureTextEx(fontTitle, text, fontSize, spacing);
    Vector2 textPos{
        drawRect.x + drawRect.width * 0.5f - textSize.x * 0.5f,
        drawRect.y + drawRect.height * 0.5f - textSize.y * 0.5f
    };

    if (hovered && !pressed)
    {
        DrawTextEx(
            fontTitle,
            text,
            Vector2{ textPos.x + 1.5f, textPos.y + 1.5f },
            fontSize,
            spacing,
            Color{ 0, 0, 0, 90 }
        );
    }

    Color textColor =
        pressed ? Color{ 80, 180, 130, 255 } :
        hovered ? Color{ 255, 255, 255, 255 } :
        Color{ 220, 200, 210, 230 };

    DrawTextEx(fontTitle, text, textPos, fontSize, spacing, textColor);
}