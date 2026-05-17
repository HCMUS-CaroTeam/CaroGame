#include "View/ui_frame.h"
#include "raylib.h"

static Texture2D gPanelFrame{};
static Texture2D gSmallFrame{};
static Texture2D gCardFrame{};

static bool gPanelLoaded = false;
static bool gSmallLoaded = false;
static bool gCardLoaded = false;
static bool gTriedLoad = false;

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

static void LoadFrameAssets()
{
    if (gTriedLoad) return;
    gTriedLoad = true;

    gPanelFrame = LoadTextureFromCandidates(
        "assets/frame/pixil-layer-1.png",
        "pixil-layer-1.png"
    );
    gPanelLoaded = IsTextureReadyEx(gPanelFrame);

    gSmallFrame = LoadTextureFromCandidates(
        "assets/frame/pixil-layer-2.png",
        "pixil-layer-2.png"
    );
    gSmallLoaded = IsTextureReadyEx(gSmallFrame);

    gCardFrame = LoadTextureFromCandidates(
        "assets/frame/pixil-layer-3.png",
        "pixil-layer-3.png"
    );
    gCardLoaded = IsTextureReadyEx(gCardFrame);
}

static void DrawFrameFallback(Rectangle dest)
{
    DrawRectangleRounded(dest, 0.12f, 10, Color{ 200, 195, 165, 255 });
    DrawRectangleRoundedLinesEx(dest, 0.12f, 10, 3.0f, Color{ 200, 30, 90, 255 });
}

static void DrawTextureFrameNPatch(const Texture2D& tex, Rectangle dest, int left, int top, int right, int bottom)
{
    if (!IsTextureReadyEx(tex))
    {
        DrawFrameFallback(dest);
        return;
    }

    NPatchInfo patch{};
    patch.source = Rectangle{
        0.0f,
        0.0f,
        static_cast<float>(tex.width),
        static_cast<float>(tex.height)
    };
    patch.left = left;
    patch.top = top;
    patch.right = right;
    patch.bottom = bottom;
    patch.layout = NPATCH_NINE_PATCH;

    DrawTextureNPatch(
        tex,
        patch,
        dest,
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

void InitUIFrameSystem()
{
    LoadFrameAssets();
}

void ShutdownUIFrameSystem()
{
    if (gPanelLoaded)
    {
        UnloadTexture(gPanelFrame);
        gPanelFrame = {};
        gPanelLoaded = false;
    }

    if (gSmallLoaded)
    {
        UnloadTexture(gSmallFrame);
        gSmallFrame = {};
        gSmallLoaded = false;
    }

    if (gCardLoaded)
    {
        UnloadTexture(gCardFrame);
        gCardFrame = {};
        gCardLoaded = false;
    }

    gTriedLoad = false;
}

bool IsUIFrameReady()
{
    return gPanelLoaded || gSmallLoaded || gCardLoaded;
}

void DrawPanelFrame(Rectangle dest)
{
    LoadFrameAssets();

    // frame lớn
    DrawTextureFrameNPatch(gPanelFrame, dest, 24, 24, 24, 24);
}

void DrawSmallFrame(Rectangle dest)
{
    LoadFrameAssets();

    // frame ngang nhỏ
    DrawTextureFrameNPatch(gSmallFrame, dest, 12, 12, 12, 12);
}

void DrawCardFrame(Rectangle dest)
{
    LoadFrameAssets();

    // frame ô / card
    DrawTextureFrameNPatch(gCardFrame, dest, 18, 18, 18, 18);
}

// Assuming your BossData struct looks something like this:
struct BossData {
    const char* title;
    const char* name;
    const char* description;
};

// Forward declaration of your existing UI frame drawing function
void DrawPanelFrame(Rectangle dest);

void DrawBossPreview(Texture2D avatar, Rectangle dialogBounds, BossData data, float scale) 
{
    // 1. Draw the background frame (using your existing UI asset system)
    DrawPanelFrame(dialogBounds);

    float halfWidth = dialogBounds.width / 2.0f;
    
    // 2. Left Pane - Avatar
    Rectangle leftPane = { dialogBounds.x, dialogBounds.y, halfWidth, dialogBounds.height };
    
    float scaledW = avatar.width * scale;
    float scaledH = avatar.height * scale;
    
    // Calculate centered coordinates for the scaled avatar
    float avatarX = leftPane.x + (leftPane.width - scaledW) / 2.0f;
    float avatarY = leftPane.y + (leftPane.height - scaledH) / 2.0f;
    
    DrawTextureEx(avatar, { avatarX, avatarY }, 0.0f, scale, WHITE);

    // 3. Right Pane - Text contents
    Rectangle rightPane = { dialogBounds.x + halfWidth, dialogBounds.y, halfWidth, dialogBounds.height };
    
    float textPaddingX = 30.0f;
    float textPaddingY = 40.0f;
    
    float currentY = rightPane.y + textPaddingY;
    
    // Draw Title (e.g., "BOSS 2")
    DrawText(data.title, static_cast<int>(rightPane.x + textPaddingX), static_cast<int>(currentY), 20, GRAY);
    currentY += 30.0f; // line spacing
    
    // Draw Name (e.g., "Half Moon Demon")
    DrawText(data.name, static_cast<int>(rightPane.x + textPaddingX), static_cast<int>(currentY), 30, DARKGRAY);
    currentY += 50.0f; // larger spacing before description
    
    // Draw Description
    // Note: If description has multiple lines or requires word-wrap, 
    // you would loop through lines or use a custom DrawTextWrap function here.
    DrawText(data.description, static_cast<int>(rightPane.x + textPaddingX), static_cast<int>(currentY), 20, BLACK);
}