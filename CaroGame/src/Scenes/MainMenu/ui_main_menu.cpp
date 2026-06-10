#include "Scenes/MainMenu/ui_main_menu.h"
#include "Scenes/Setting/ui_setting.h"
#include "View/ui_button.h"
#include "Model/config.h"
#include <cmath>

static Texture2D gMenuBg{};
static Texture2D gMenuLogo{};
static Texture2D gMenuButtonIdle{};
static Texture2D gMenuButtonHover{};
static Texture2D gMenuButtonPressed{};
static Texture2D gMenuPipIdle{};
static Texture2D gMenuPipHover{};
static bool gMenuAssetsLoaded = false;

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

static void LoadMainMenuAssets()
{
    if (gMenuAssetsLoaded)
    {
        return;
    }

    gMenuBg = LoadTextureFromCandidates("assets/bg/main_menu.png", "Assets/bg/main_menu.png");
    gMenuLogo = LoadTextureFromCandidates("assets/bg/game name.png", "Assets/bg/game name.png");
    gMenuButtonIdle = LoadTextureFromCandidates("assets/buttons/button1.png", "Assets/buttons/button1.png");
    gMenuButtonHover = LoadTextureFromCandidates("assets/buttons/pixil-layer-9.png", "Assets/buttons/pixil-layer-9.png");
    gMenuButtonPressed = LoadTextureFromCandidates("assets/buttons/button2.png", "Assets/buttons/button2.png");
    gMenuPipIdle = LoadTextureFromCandidates("assets/buttons/pixil-layer-12.png", "Assets/buttons/pixil-layer-12.png");
    gMenuPipHover = LoadTextureFromCandidates("assets/buttons/pixil-layer-13.png", "Assets/buttons/pixil-layer-13.png");

    gMenuAssetsLoaded = true;
}

static void UnloadMainMenuAssets()
{
    if (IsTextureReadyEx(gMenuBg)) UnloadTexture(gMenuBg);
    if (IsTextureReadyEx(gMenuLogo)) UnloadTexture(gMenuLogo);
    if (IsTextureReadyEx(gMenuButtonIdle)) UnloadTexture(gMenuButtonIdle);
    if (IsTextureReadyEx(gMenuButtonHover)) UnloadTexture(gMenuButtonHover);
    if (IsTextureReadyEx(gMenuButtonPressed)) UnloadTexture(gMenuButtonPressed);
    if (IsTextureReadyEx(gMenuPipIdle)) UnloadTexture(gMenuPipIdle);
    if (IsTextureReadyEx(gMenuPipHover)) UnloadTexture(gMenuPipHover);

    gMenuBg = {};
    gMenuLogo = {};
    gMenuButtonIdle = {};
    gMenuButtonHover = {};
    gMenuButtonPressed = {};
    gMenuPipIdle = {};
    gMenuPipHover = {};
    gMenuAssetsLoaded = false;
}

static float Fract(float value)
{
    return value - floorf(value);
}

static Rectangle GetCoverSourceRect(const Texture2D& tex)
{
    const float screenRatio = static_cast<float>(SCREEN_WIDTH) / static_cast<float>(SCREEN_HEIGHT);
    const float textureRatio = static_cast<float>(tex.width) / static_cast<float>(tex.height);

    if (textureRatio > screenRatio)
    {
        const float sourceWidth = static_cast<float>(tex.height) * screenRatio;
        return Rectangle{
            (static_cast<float>(tex.width) - sourceWidth) * 0.5f,
            0.0f,
            sourceWidth,
            static_cast<float>(tex.height)
        };
    }

    const float sourceHeight = static_cast<float>(tex.width) / screenRatio;
    return Rectangle{
        0.0f,
        (static_cast<float>(tex.height) - sourceHeight) * 0.5f,
        static_cast<float>(tex.width),
        sourceHeight
    };
}

static void DrawTextureCover(const Texture2D& tex)
{
    DrawTexturePro(
        tex,
        GetCoverSourceRect(tex),
        Rectangle{ 0.0f, 0.0f, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT) },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

static void DrawMainMenuBackground()
{
    LoadMainMenuAssets();
    ClearBackground(Color{ 6, 4, 15, 255 });

    if (IsTextureReadyEx(gMenuBg))
    {
        DrawTextureCover(gMenuBg);
    }

    DrawRectangleGradientH(
        0,
        0,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        Color{ 6, 4, 15, 188 },
        Color{ 6, 4, 15, 0 }
    );

    DrawRectangleGradientV(
        0,
        SCREEN_HEIGHT - 300,
        SCREEN_WIDTH,
        300,
        Color{ 6, 4, 15, 0 },
        Color{ 6, 4, 15, 205 }
    );

    const float pulse = 0.82f + sinf(static_cast<float>(GetTime()) * 1.25f) * 0.12f;
    DrawCircleGradient(
        SCREEN_WIDTH - 350,
        210,
        420.0f * pulse,
        Color{ 180, 20, 10, 46 },
        Color{ 180, 20, 10, 0 }
    );

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 32 });
}

static void DrawMainMenuParticles()
{
    const float time = static_cast<float>(GetTime());

    for (int i = 0; i < 34; ++i)
    {
        const float seed = static_cast<float>(i) * 17.37f;
        const float life = Fract(time * (0.045f + Fract(seed) * 0.035f) + Fract(seed * 0.31f));
        const float x = 70.0f + Fract(seed * 4.91f) * (SCREEN_WIDTH - 140.0f) + sinf(time + seed) * 18.0f;
        const float y = SCREEN_HEIGHT + 25.0f - life * (SCREEN_HEIGHT + 120.0f);
        const float radius = 1.0f + Fract(seed * 2.13f) * 2.5f;
        float alphaFactor = 1.0f - fabsf(life - 0.45f) / 0.45f;

        if (alphaFactor < 0.0f)
        {
            alphaFactor = 0.0f;
        }

        const unsigned char alpha = static_cast<unsigned char>(alphaFactor * 95.0f);
        DrawCircleV(Vector2{ x, y }, radius, Color{ 220, 42, 80, alpha });
    }
}

static void DrawMainMenuScanlines()
{
    for (int y = 0; y < SCREEN_HEIGHT; y += 4)
    {
        DrawRectangle(0, y + 2, SCREEN_WIDTH, 1, Color{ 0, 0, 0, 18 });
    }
}

static void DrawMainMenuLogo()
{
    if (!IsTextureReadyEx(gMenuLogo))
    {
        return;
    }

    const float time = static_cast<float>(GetTime());
    const float targetWidth = 1090.0f;
    const float scale = targetWidth / static_cast<float>(gMenuLogo.width);
    const float targetHeight = static_cast<float>(gMenuLogo.height) * scale;
    const float floatOffset = sinf(time * 1.05f) * 7.0f;
    const float logoX = SCREEN_WIDTH * 0.5f - targetWidth * 0.5f;

    DrawTexturePro(
        gMenuLogo,
        Rectangle{ 0.0f, 0.0f, static_cast<float>(gMenuLogo.width), static_cast<float>(gMenuLogo.height) },
        Rectangle{ logoX, 160.0f + floatOffset, targetWidth, targetHeight },
        Vector2{ 0.0f, 0.0f },
        0.0f,
        WHITE
    );
}

static const Texture2D* GetMenuButtonTexture(bool hovered, bool pressed)
{
    if (pressed && IsTextureReadyEx(gMenuButtonPressed)) return &gMenuButtonPressed;
    if (hovered && IsTextureReadyEx(gMenuButtonHover)) return &gMenuButtonHover;
    if (IsTextureReadyEx(gMenuButtonIdle)) return &gMenuButtonIdle;
    return nullptr;
}

static void DrawTextureIntoRect(const Texture2D& tex, Rectangle dest, Color tint)
{
    DrawTexturePro(
        tex,
        Rectangle{ 0.0f, 0.0f, static_cast<float>(tex.width), static_cast<float>(tex.height) },
        dest,
        Vector2{ 0.0f, 0.0f },
        0.0f,
        tint
    );
}

static void DrawMainMenuDivider(Rectangle upperButton, Rectangle lowerButton)
{
    const float y = (upperButton.y + upperButton.height + lowerButton.y) * 0.5f;
    const float menuCenterX = 110.0f + 266.0f * 0.5f;
    const int width = 210;
    const int startX = static_cast<int>(menuCenterX - width * 0.5f);
    const int drawY = static_cast<int>(y);

    for (int i = 0; i < width; ++i)
    {
        const float distanceFromCenter = fabsf(i - width * 0.5f) / (width * 0.5f);
        const float alphaFactor = 1.0f - distanceFromCenter;
        const unsigned char alpha = static_cast<unsigned char>(alphaFactor * 130.0f);
        DrawPixel(startX + i, drawY, Color{ 196, 24, 64, alpha });
    }
}

static void DrawMainMenuButton(const Button& button, Font fontTitle, bool hovered, bool pressed)
{
    Rectangle rect = GetButtonRect(button);

    if (pressed)
    {
        rect.y += 3.0f;
    }
    else if (hovered)
    {
        rect.y -= 4.0f;
    }

    const Texture2D* buttonTex = GetMenuButtonTexture(hovered, pressed);
    if (buttonTex)
    {
        DrawTextureIntoRect(*buttonTex, rect, WHITE);
    }
    else
    {
        DrawRectangleRounded(rect, 0.18f, 8, Color{ 70, 12, 32, 255 });
        DrawRectangleRoundedLinesEx(rect, 0.18f, 8, 2.0f, Color{ 196, 24, 64, 255 });
    }

    const char* text = button.title.c_str();
    const Vector2 textSize = MeasureTextEx(fontTitle, text, button.fontSize, button.textSpacing);
    const Vector2 textPos{
        rect.x + rect.width * 0.5f - textSize.x * 0.5f,
        rect.y + rect.height * 0.5f - textSize.y * 0.5f - 5.0f
    };

    DrawTextEx(
        fontTitle,
        text,
        Vector2{ textPos.x + 2.0f, textPos.y + 2.0f },
        button.fontSize,
        button.textSpacing,
        Color{ 0, 0, 0, 150 }
    );

    const Color textColor = pressed ? Color{ 90, 190, 130, 255 } :
        hovered ? Color{ 255, 255, 255, 255 } :
        Color{ 232, 216, 168, 245 };

    DrawTextEx(fontTitle, text, textPos, button.fontSize, button.textSpacing, textColor);
}

static void DrawMainMenuHud(Font fontSmall)
{
    DrawTextEx(fontSmall, "v1.0", Vector2{ 110.0f, 862.0f }, 13.0f, 2.0f, Color{ 168, 152, 112, 90 });

}

void InitMainMenuUI()
{
    InitUIButtonSystem();
    LoadMainMenuAssets();
}

void ShutdownMainMenuUI()
{
    UnloadMainMenuAssets();
    ShutdownUIButtonSystem();
}

static void SetupFlexibleButtonLayout()
{
    // Cấu hình kích thước đồng bộ cho các nút trên hàng ngang
    const float normW = 200.0f; // Độ rộng nút thường
    const float normH = 62.0f;  // Chiều cao nút thường
    const float playW = 245.0f; // Nút PLAY làm bự hơn để tạo điểm nhấn ở giữa
    const float playH = 75.0f;

    // Cao độ trục Y cho cả hàng 
    const float rowY = 680.0f;
    const float spacingX = 25.0f; // Khoảng cách ngang giữa các nút

    // Tâm màn hình trục X
    const float centerX = SCREEN_WIDTH * 0.5f;

    // Tổng độ rộng của cả cụm 5 nút (4 nút thường + 1 nút PLAY + 4 khoảng cách)
    const float totalMenuWidth = (normW * 4.0f) + playW + (spacingX * 4.0f);

    // Điểm xuất phát X đầu tiên ở cạnh bên trái cùng của hàng
    const float startX = centerX - (totalMenuWidth * 0.5f);

    for (int i = 0; i < gMainMenuButtonCount; ++i)
    {
        // Gán font size mặc định cho các nút thường
        gMainMenuButtons[i].fontSize = 20.0f;

        switch (gMainMenuButtons[i].id)
        {
            // Vị trí 1: LOAD (Bên trái cùng)
        case BTN_LOAD:
            gMainMenuButtons[i].size.x = normW;
            gMainMenuButtons[i].size.y = normH;
            gMainMenuButtons[i].position.x = startX;
            gMainMenuButtons[i].position.y = rowY;
            break;

            // Vị trí 2: SETTING
        case BTN_SETTING:
            gMainMenuButtons[i].size.x = normW;
            gMainMenuButtons[i].size.y = normH;
            // startX + nút LOAD + 1 khoảng cách
            gMainMenuButtons[i].position.x = startX + normW + spacingX;
            gMainMenuButtons[i].position.y = rowY;
            break;

            // Vị trí 3: PLAY (NẰM CHÍNH GIỮA HÀNG)
        case BTN_PLAY:
            gMainMenuButtons[i].size.x = playW;
            gMainMenuButtons[i].size.y = playH;
            // startX + nút LOAD + nút SETTING + 2 khoảng cách
            gMainMenuButtons[i].position.x = startX + (normW * 2.0f) + (spacingX * 2.0f);
            gMainMenuButtons[i].position.y = rowY - (playH - normH) * 0.5f; // Căn giữa trục Y với các nút cạnh
            gMainMenuButtons[i].fontSize = 24.0f; // Chữ to hơn cho nổi bật
            break;

            // Vị trí 4: ABOUT
        case BTN_ABOUT:
            gMainMenuButtons[i].size.x = normW;
            gMainMenuButtons[i].size.y = normH;
            // Vị trí sau nút PLAY: startX + 2 nút thường + 1 nút PLAY + 3 khoảng cách
            gMainMenuButtons[i].position.x = startX + (normW * 2.0f) + playW + (spacingX * 3.0f);
            gMainMenuButtons[i].position.y = rowY;
            break;

            // Vị trí 5: EXIT (Bên phải cùng)
        case BTN_EXIT:
            gMainMenuButtons[i].size.x = normW;
            gMainMenuButtons[i].size.y = normH;
            // Vị trí sau nút ABOUT: startX + 3 nút thường + 1 nút PLAY + 4 khoảng cách
            gMainMenuButtons[i].position.x = startX + (normW * 3.0f) + playW + (spacingX * 4.0f);
            gMainMenuButtons[i].position.y = rowY;
            break;
        }

        // Cập nhật lại struct Rectangle tương ứng để đồng bộ hitbox click
        gMainMenuButtons[i].position.x = gMainMenuButtons[i].position.x;
        gMainMenuButtons[i].position.y = gMainMenuButtons[i].position.y;
        gMainMenuButtons[i].size.x = gMainMenuButtons[i].size.x;
        gMainMenuButtons[i].size.y = gMainMenuButtons[i].size.y;
    }
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
    SetupFlexibleButtonLayout();

    for (int i = 0; i < gMainMenuButtonCount; ++i)
    {
        bool hovered = false;
        bool pressed = false;

        UpdateUIButton(
            i,
            gMainMenuButtons[i],
            mouse,
            dt,
            audio,
            settings,
            hovered,
            pressed
        );

        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);

            switch (gMainMenuButtons[i].id)
            {
            case BTN_PLAY:
                currentScreen = SCREEN_STORY; // thêm scene story 
                break;

            case BTN_LOAD:
                currentScreen = SCREEN_LOAD;
                break;

            case BTN_ABOUT:
                currentScreen = SCREEN_ABOUT;
                break;

            case BTN_SETTING:
                SetSettingReturnScreen(SCREEN_MAIN_MENU);
                currentScreen = SCREEN_SETTING;
                break;

            case BTN_EXIT:
                shouldClose = true;
                break;
            }
        }
    }
}

void DrawMainMenuUI(Font fontTitle, Font fontSmall, const MouseState& mouse)
{
    SetupFlexibleButtonLayout();

    DrawMainMenuBackground();
    DrawMainMenuParticles();
    DrawMainMenuLogo();

    for (int i = 0; i < gMainMenuButtonCount; ++i)
    {
        const Rectangle hitRect = GetButtonRect(gMainMenuButtons[i]);
        const bool hovered = IsMouseOverRect(mouse, hitRect);
        const bool pressed = hovered && mouse.leftDown;
        DrawMainMenuButton(gMainMenuButtons[i], fontTitle, hovered, pressed);
    }

    DrawMainMenuScanlines();
    DrawMainMenuHud(fontSmall);
}
