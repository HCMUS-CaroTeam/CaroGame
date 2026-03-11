#include "ui_background.h"
#include "config.h"
#include "colors.h"
#include "ui_fx.h"
#include "raylib.h"

void DrawBackgroundScene()
{
    ClearBackground(COLOR_BG_DARK);

    DrawCircleGradient(
        SCREEN_WIDTH / 2,
        SCREEN_HEIGHT / 2,
        560.0f,
        FadeColorEx(COLOR_NEON_BLUE, 0.14f),
        COLOR_BG_DARK
    );

    DrawCircleGradient(
        280,
        130,
        250.0f,
        FadeColorEx(COLOR_NEON_CYAN, 0.08f),
        FadeColorEx(COLOR_BG_DARK, 0.0f)
    );

    for (int x = 0; x < SCREEN_WIDTH; x += 40)
        DrawLine(x, 0, x, SCREEN_HEIGHT, FadeColorEx(COLOR_GRID, 0.05f));

    for (int y = 0; y < SCREEN_HEIGHT; y += 40)
        DrawLine(0, y, SCREEN_WIDTH, y, FadeColorEx(COLOR_GRID, 0.05f));

    for (int x = 0; x < SCREEN_WIDTH; x += 160)
        DrawLine(x, 0, x, SCREEN_HEIGHT, FadeColorEx(COLOR_NEON_BLUE, 0.07f));

    for (int y = 0; y < SCREEN_HEIGHT; y += 160)
        DrawLine(0, y, SCREEN_WIDTH, y, FadeColorEx(COLOR_NEON_BLUE, 0.07f));
}