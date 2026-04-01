#include "raylib.h"
#include "Model/config.h"
#include "Control/input_mouse.h"
#include "Scenes/MainMenu/ui_main_menu.h"
#include "Scenes/MainMenu/ui_background.h"
#include "audio_manager.h"
#include "Model/app_settings.h"
#include "Control/menu_data.h"

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
    DrawBackgroundScene();

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
    DrawCenteredText(fontSmall, "PRESS ESC OR LEFT CLICK TO BACK", 515.0f, 22.0f, Color{ 220, 205, 205, 170 });
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Quantum Caro");
    SetTargetFPS(TARGET_FPS);

    // === DEBUG: xóa sau khi tìm ra lỗi ===
    // In ra thư mục làm việc hiện tại
    TraceLog(LOG_INFO, "Working dir: %s", GetWorkingDirectory());

    // Kiểm tra từng file ảnh
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

    ScreenState currentScreen = SCREEN_MAIN_MENU;
    bool shouldClose = false;

    bool showMenuButtons = true; // false = cất nút

    while (!WindowShouldClose() && !shouldClose)
    {
        const MouseState mouse = GetMouseStateNow();
        const float dt = GetFrameTime();

        UpdateGameAudio(audio, settings);

        if (currentScreen == SCREEN_MAIN_MENU)
        {
            if (showMenuButtons)
            {
                UpdateMainMenuUI(mouse, dt, audio, settings, currentScreen, shouldClose);
            }

            if (IsKeyPressed(KEY_ESCAPE))
            {
                shouldClose = true;
            }
        }

        BeginDrawing();

        switch (currentScreen)
        {
        case SCREEN_MAIN_MENU:
            if (showMenuButtons)
            {
                DrawMainMenuUI(fontTitle, fontSmall, mouse);
            }
            else
            {
                // chỉ vẽ background + logo
                DrawBackgroundScene();
            }
            break;

        case SCREEN_PLAY:
            DrawSubScreen(fontTitle, fontSmall, "PLAY", "This screen is ready for your next game scene.");
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

    ShutdownMainMenuUI();
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