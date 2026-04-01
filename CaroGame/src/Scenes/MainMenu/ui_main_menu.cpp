#include "Scenes/MainMenu/ui_main_menu.h"
#include "Scenes/MainMenu/ui_background.h"
#include "Model/config.h"
#include <algorithm>

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
// (đo bằng script — loại bỏ vùng đen trong suốt xung quanh)
// button1.png (idle  - đỏ  ): x=211..287, y=180..212 → 77x33
// button2.png (press - xanh): x=319..393, y=178..212 → 75x35
static constexpr Rectangle SRC_IDLE = { 211.0f, 180.0f, 77.0f, 33.0f };
static constexpr Rectangle SRC_PRESSED = { 319.0f, 178.0f, 75.0f, 35.0f };

// Kích thước nút hiển thị trên màn hình
static constexpr float BTN_W = 280.0f;
static constexpr float BTN_H = 80.0f;

// ─────────────────────────────────────────────
static bool IsTextureReadyEx(const Texture2D& tex) { return tex.id != 0; }

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
            TraceLog(LOG_WARNING, "BUTTON: Khong load duoc button1.png");
    }
    if (!gButtonPressedAttempted)
    {
        gButtonPressedAttempted = true;
        gButtonPressed = LoadTextureFromCandidates("assets/bg/button2.png", "button2.png");
        gButtonPressedLoaded = IsTextureReadyEx(gButtonPressed);
        if (!gButtonPressedLoaded)
            TraceLog(LOG_WARNING, "BUTTON: Khong load duoc button2.png");
    }
}

static Rectangle GetBaseButtonRect(const NeonButton& button)
{
    return Rectangle{ button.position.x, button.position.y, BTN_W, BTN_H };
}

static Rectangle GetDrawButtonRect(const NeonButton& button, bool hovered, bool pressed, float hoverAnim)
{
    Rectangle rect = GetBaseButtonRect(button);

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
        rect.y += 3.0f;

    return rect;
}

// Vẽ texture crop đúng vùng nút thật → scale vừa dest
static void DrawButtonTextureCropped(const Texture2D& tex, Rectangle src, Rectangle dest)
{
    DrawTexturePro(tex, src, dest, Vector2{ 0.0f, 0.0f }, 0.0f, WHITE);
}

static void UpdateButtonAnim(int index, bool hovered, bool pressed, float dt)
{
    const float hoverTarget = hovered ? 1.0f : 0.0f;
    const float pressTarget = pressed ? 1.0f : 0.0f;

    if (gHoverAnim[index] < hoverTarget)
        gHoverAnim[index] = std::min(hoverTarget, gHoverAnim[index] + dt * 8.0f);
    else
        gHoverAnim[index] = std::max(hoverTarget, gHoverAnim[index] - dt * 8.0f);

    if (gPressAnim[index] < pressTarget)
        gPressAnim[index] = std::min(pressTarget, gPressAnim[index] + dt * 16.0f);
    else
        gPressAnim[index] = std::max(pressTarget, gPressAnim[index] - dt * 16.0f);
}

void InitMainMenuUI()
{
    LoadButtonAssets();
    for (int i = 0; i < 32; ++i)
    {
        gWasHover[i] = false;
        gHoverAnim[i] = 0.0f;
        gPressAnim[i] = 0.0f;
    }
}

void ShutdownMainMenuUI()
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

void UpdateMainMenuUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
)
{
    for (int i = 0; i < gButtonCount; ++i)
    {
        const Rectangle hitRect = GetBaseButtonRect(gButtons[i]);
        const bool hovered = IsMouseOverRect(mouse, hitRect);
        const bool pressed = hovered && mouse.leftDown;

        if (hovered && !gWasHover[i])
            PlayMenuHover(audio);

        UpdateButtonAnim(i, hovered, pressed, dt);
        gWasHover[i] = hovered;

        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);
            switch (gButtons[i].id)
            {
            case BTN_PLAY:    currentScreen = SCREEN_PLAY;    break;
            case BTN_ABOUT:   currentScreen = SCREEN_ABOUT;   break;
            case BTN_SETTING: currentScreen = SCREEN_SETTING; break;
            case BTN_EXIT:    shouldClose = true;           break;
            }
        }
    }
}

// ─────────────────────────────────────────────
static void DrawMenuButton(const NeonButton& button, Font fontTitle, bool hovered, bool pressed, float hoverAnim)
{
    const Rectangle drawRect = GetDrawButtonRect(button, hovered, pressed, hoverAnim);

    // Vẽ texture crop đúng vùng nút thật → scale lên vừa drawRect
    if (pressed && gButtonPressedLoaded)
        DrawButtonTextureCropped(gButtonPressed, SRC_PRESSED, drawRect);
    else if (gButtonIdleLoaded)
        DrawButtonTextureCropped(gButtonIdle, SRC_IDLE, drawRect);

    // ── Chữ ──
    const char* text = button.title.c_str();
    const float fontSize = 28.0f;
    const float spacing = 2.0f;

    Vector2 textSize = MeasureTextEx(fontTitle, text, fontSize, spacing);
    Vector2 textPos{
        drawRect.x + drawRect.width * 0.5f - textSize.x * 0.5f,
        drawRect.y + drawRect.height * 0.5f - textSize.y * 0.5f
    };

    if (hovered && !pressed)
        DrawTextEx(fontTitle, text,
            Vector2{ textPos.x + 1.5f, textPos.y + 1.5f },
            fontSize, spacing, Color{ 0, 0, 0, 90 });

    Color textColor = pressed ? Color{ 80, 180, 130, 255 }
        : hovered ? Color{ 255, 255, 255, 255 }
    : Color{ 220, 200, 210, 230 };

    DrawTextEx(fontTitle, text, textPos, fontSize, spacing, textColor);
}

void DrawMainMenuUI(Font fontTitle, Font fontSmall, const MouseState& mouse)
{
    DrawBackgroundScene();

    for (int i = 0; i < gButtonCount; ++i)
    {
        const Rectangle hitRect = GetBaseButtonRect(gButtons[i]);
        const bool hovered = IsMouseOverRect(mouse, hitRect);
        const bool pressed = hovered && mouse.leftDown;
        DrawMenuButton(gButtons[i], fontTitle, hovered, pressed, gHoverAnim[i]);
    }
}