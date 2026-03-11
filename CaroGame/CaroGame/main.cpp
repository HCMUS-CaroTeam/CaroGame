#include "raylib.h"
#include "config.h"
#include "colors.h"
#include "menu_data.h"
#include "ui_background.h"
#include "ui_button.h"
#include "ui_panel.h"
#include "ui_fx.h"
#include <algorithm>

static Font LoadFontSafe(const char* path, int size)
{
    if (FileExists(path))
        return LoadFontEx(path, size, 0, 0);

    return GetFontDefault();
}

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Quantum Caro");
    SetTargetFPS(TARGET_FPS);

    Font fontTitle = LoadFontSafe(FONT_PATH, 64);
    Font fontSmall = LoadFontSafe(FONT_PATH, 28);

    float hoverAnim[32] = {};
    float clickAnim[32] = {};
    int selectedIndex = 0;

    while (!WindowShouldClose())
    {
        MouseState mouse = GetMouseStateNow();
        float dt = GetFrameTime();

        for (int i = 0; i < gButtonCount; ++i)
        {
            bool mouseHover = IsButtonHovered(gButtons[i], mouse);

            float target = mouseHover ? 1.0f : 0.0f;
            float speed = mouseHover ? 8.5f : 6.0f;

            hoverAnim[i] += (target - hoverAnim[i]) * std::min(1.0f, dt * speed);

            if (IsButtonClicked(gButtons[i], mouse))
            {
                clickAnim[i] = 1.0f;
                selectedIndex = i;
            }

            clickAnim[i] = std::max(0.0f, clickAnim[i] - dt * 3.8f);
        }

        if (IsButtonClicked(gButtons[BTN_EXIT], mouse))
        {
            break;
        }

        BeginDrawing();

        DrawBackgroundScene();
        DrawLeftModePanel(fontTitle, fontSmall);

        DrawTextEx(fontTitle, "QUANTUM CARO:", Vector2{ 480, 90 }, 44.0f, 2.0f, COLOR_TEXT_MAIN);
        DrawTextEx(fontTitle, "PROTOCOL 5", Vector2{ 850, 90 }, 44.0f, 2.0f, COLOR_TEXT_MAIN);
        DrawTextEx(fontSmall, "X O X X O", Vector2{ 773, 150 }, 24.0f, 4.0f, FadeColorEx(COLOR_NEON_CYAN, 0.9f));

        for (int i = 0; i < gButtonCount; ++i)
        {
            DrawNeonButton(
                gButtons[i],
                false,
                mouse,
                hoverAnim[i],
                clickAnim[i],
                fontTitle,
                fontSmall
            );
        }

        EndDrawing();
    }

    if (FileExists(FONT_PATH))
    {
        UnloadFont(fontTitle);
        UnloadFont(fontSmall);
    }

    CloseWindow();
    return 0;
}