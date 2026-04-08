#include "raylib.h"
#include "Model/config.h"
#include "Control/input_mouse.h"
#include "Scenes/MainMenu/ui_main_menu.h"
#include "View/ui_background.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"
#include "Control/menu_data.h"
#include "Scenes/Setup/ui_setup.h"
#include "Scenes/Play/ui_play.h"

static Font LoadFontSafe(const char* path, int size)
{
    if (FileExists(path))
        return LoadFontEx(path, size, 0, 0);

    return GetFontDefault();
}

static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(
        font,
        text,
        Vector2{ SCREEN_WIDTH * 0.5f - size.x * 0.5f, y },
        fontSize,
        1.0f,
        color
    );
}

static void DrawSubScreen(Font fontTitle, Font fontSmall, const char* title, const char* desc)
{
    DrawBackgroundOnly();
    DrawLogoOnly();

    Rectangle panel{
        SCREEN_WIDTH * 0.5f - 360.0f,
        360.0f,
        720.0f,
        180.0f
    };

    DrawRectangleRounded(panel, 0.08f, 12, Color{ 28, 16, 20, 180 });
    DrawRectangleRoundedLinesEx(panel, 0.08f, 12, 2.5f, Color{ 235, 210, 210, 200 });

    DrawCenteredText(fontTitle, title, 395.0f, 42.0f, Color{ 255, 235, 225, 255 });
    DrawCenteredText(fontSmall, desc, 465.0f, 24.0f, Color{ 240, 225, 225, 220 });
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Quantum Caro");
    SetExitKey(KEY_NULL);
    SetTargetFPS(TARGET_FPS);

    TraceLog(LOG_INFO, "Working dir: %s", GetWorkingDirectory());
    TraceLog(LOG_INFO, "button1.png (assets/bg/)  : %d", FileExists("assets/bg/button1.png"));
    TraceLog(LOG_INFO, "button1.png (Assets/bg/)  : %d", FileExists("Assets/bg/button1.png"));
    TraceLog(LOG_INFO, "button1.png (root)        : %d", FileExists("button1.png"));
    TraceLog(LOG_INFO, "background.png (assets/bg): %d", FileExists("assets/bg/background.png"));

    Font fontTitle = LoadFontSafe(FONT_PATH, 64);
    Font fontSmall = LoadFontSafe(FONT_PATH, 28);

    AppSettings settings{};
    AudioAssets audio{};
    InitGameAudio(audio);
    InitMainMenuUI();
    InitSetupUI();
    InitPlayUI();

    ScreenState currentScreen = SCREEN_MAIN_MENU;
    bool shouldClose = false;

    while (!WindowShouldClose() && !shouldClose)
    {
        const MouseState mouse = GetMouseStateNow();
        const float dt = GetFrameTime();

        UpdateGameAudio(audio, settings);

        switch (currentScreen)
        {
        case SCREEN_MAIN_MENU:
            UpdateMainMenuUI(mouse, dt, audio, settings, currentScreen, shouldClose);
            break;

        case SCREEN_SETUP:
            UpdateSetupUI(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_PLAY:
            UpdatePlayUI(mouse, dt, audio, settings, currentScreen, shouldClose);
            break;

        case SCREEN_ABOUT:
        case SCREEN_SETTING:
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = SCREEN_MAIN_MENU;
            }
            break;
        }

        BeginDrawing();

        switch (currentScreen)
        {
        case SCREEN_MAIN_MENU:
            DrawMainMenuUI(fontTitle, fontSmall, mouse);
            break;

        case SCREEN_SETUP:
            DrawSetupUI(fontTitle, fontSmall, mouse);
            break;

        case SCREEN_PLAY:
            DrawPlayUI(fontTitle, fontSmall, mouse);
            break;

        case SCREEN_ABOUT:
            DrawSubScreen(fontTitle, fontSmall, "ABOUT", "Introduce game information, team, rules, or credits here.");
            break;

        case SCREEN_SETTING:
            DrawSubScreen(fontTitle, fontSmall, "SETTING", "Audio, graphics, and gameplay settings can be built here.");
            break;
        }

        EndDrawing();
    }

    ShutdownPlayUI();
    ShutdownMainMenuUI();
    ShutdownSetupUI();
    ShutdownGameAudio(audio);
    UnloadBackgroundAssets();

    if (FileExists(FONT_PATH))
    {
        UnloadFont(fontTitle);
        UnloadFont(fontSmall);
    }

    CloseWindow();
    return 0;
}