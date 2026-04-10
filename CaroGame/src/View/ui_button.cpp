#include "View/ui_button.h"
#include <algorithm>

using namespace std;

struct ButtonTextureAsset
{
    Texture2D texture{};
    bool loaded = false;
    bool attempted = false;
};

static ButtonTextureAsset gButtonIdle;
static ButtonTextureAsset gButtonPressed;
static ButtonTextureAsset gIconArrowRightPressed;
static ButtonTextureAsset gIconArrowLeftPressed;
static ButtonTextureAsset gIconArrowLeftIdle;
static ButtonTextureAsset gIconArrowRightIdle;

static bool  gWasHover[32] = {};
static float gHoverAnim[32] = {};
static float gPressAnim[32] = {};

static constexpr Rectangle SRC_IDLE = { 211.0f, 180.0f, 77.0f, 33.0f };
static constexpr Rectangle SRC_PRESSED = { 319.0f, 178.0f, 75.0f, 35.0f };

static bool IsTextureReadyEx(const Texture2D& tex)
{
    return tex.id != 0;
}

static Texture2D LoadTextureFromCandidates(const char* a, const char* b = nullptr)
{
    if (a && FileExists(a))
    {
        return LoadTexture(a);
    }

    if (b && FileExists(b))
    {
        return LoadTexture(b);
    }

    return Texture2D{};
}

static void LoadTextureAsset(
    ButtonTextureAsset& asset,
    const char* pathA,
    const char* pathB,
    const char* debugName
)
{
    if (asset.attempted)
    {
        return;
    }

    asset.attempted = true;
    asset.texture = LoadTextureFromCandidates(pathA, pathB);
    asset.loaded = IsTextureReadyEx(asset.texture);

    if (!asset.loaded)
    {
        TraceLog(LOG_WARNING, "BUTTON: Khong load duoc %s", debugName);
    }
}

static void UnloadTextureAsset(ButtonTextureAsset& asset)
{
    if (asset.loaded)
    {
        UnloadTexture(asset.texture);
        asset.texture = {};
        asset.loaded = false;
    }

    asset.attempted = false;
}

static void LoadButtonAssets()
{
    LoadTextureAsset(gButtonIdle, "assets/buttons/button1.png", "button1.png", "button1.png");
    LoadTextureAsset(gButtonPressed, "assets/buttons/button2.png", "button2.png", "button2.png");

    LoadTextureAsset(gIconArrowRightIdle, "assets/buttons/pixil-layer-10.png", "pixil-layer-10.png", "pixil-layer-10.png");
    LoadTextureAsset(gIconArrowLeftIdle, "assets/buttons/pixil-layer-11.png", "pixil-layer-11.png", "pixil-layer-11.png");
    LoadTextureAsset(gIconArrowLeftPressed, "assets/buttons/pixil-layer-12.png", "pixil-layer-12.png", "pixil-layer-12.png");
    LoadTextureAsset(gIconArrowRightPressed, "assets/buttons/pixil-layer-13.png", "pixil-layer-13.png", "pixil-layer-13.png");
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

static void DrawButtonTextureFull(const Texture2D& tex, Rectangle dest)
{
    DrawTexturePro(
        tex,
        Rectangle{ 0.0f, 0.0f, static_cast<float>(tex.width), static_cast<float>(tex.height) },
        dest,
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

static const Texture2D* GetIconTexture(ButtonIconType iconType, bool pressed)
{
    switch (iconType)
    {
    case BUTTON_ICON_ARROW_RIGHT:
        if (pressed)
        {
            return gIconArrowRightPressed.loaded ? &gIconArrowRightPressed.texture : nullptr;
        }
        return gIconArrowRightIdle.loaded ? &gIconArrowRightIdle.texture : nullptr;

    case BUTTON_ICON_ARROW_LEFT:
        if (pressed)
        {
            return gIconArrowLeftPressed.loaded ? &gIconArrowLeftPressed.texture : nullptr;
        }
        return gIconArrowLeftIdle.loaded ? &gIconArrowLeftIdle.texture : nullptr;

    default:
        return nullptr;
    }
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
    UnloadTextureAsset(gButtonIdle);
    UnloadTextureAsset(gButtonPressed);
    UnloadTextureAsset(gIconArrowRightPressed);
    UnloadTextureAsset(gIconArrowLeftPressed);
    UnloadTextureAsset(gIconArrowLeftIdle);
    UnloadTextureAsset(gIconArrowRightIdle);
}

Rectangle GetButtonRect(const Button& button)
{
    return Rectangle{
        button.position.x,
        button.position.y,
        button.size.x,
        button.size.y
    };
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
    (void)settings;

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

    if (button.visualType == BUTTON_VISUAL_ICON)
    {
        const Texture2D* iconTex = GetIconTexture(button.iconType, pressed);
        if (iconTex)
        {
            DrawButtonTextureFull(*iconTex, drawRect);
        }
        return;
    }

    if (pressed && gButtonPressed.loaded)
    {
        DrawButtonTextureCropped(gButtonPressed.texture, SRC_PRESSED, drawRect);
    }
    else if (gButtonIdle.loaded)
    {
        DrawButtonTextureCropped(gButtonIdle.texture, SRC_IDLE, drawRect);
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