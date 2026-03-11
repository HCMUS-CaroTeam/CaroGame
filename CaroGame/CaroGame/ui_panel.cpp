#include "ui_panel.h"
#include "colors.h"
#include "ui_fx.h"
#include "raylib.h"

static void DrawInfoCell(Font font, Rectangle rect, const char* left, const char* right, Color accent)
{
    DrawRectangleRec(rect, FadeColorEx(Color{ 8,16,30,255 }, 0.96f));
    DrawCutCornerFrame(rect, 8.0f, 1.2f, accent, 0.65f);

    DrawTextEx(font, left, Vector2{ rect.x + 12.0f, rect.y + 8.0f }, 17.0f, 1.0f, FadeColorEx(COLOR_TEXT_DIM, 0.95f));
    DrawTextEx(font, right, Vector2{ rect.x + 115.0f, rect.y + 8.0f }, 17.0f, 1.0f, FadeColorEx(COLOR_TEXT_MAIN, 0.95f));
}

void DrawLeftModePanel(Font fontTitle, Font fontSmall)
{
    Rectangle outer{ 70, 90, 300, 560 };
    Rectangle inner{ 78, 98, 284, 544 };

    DrawGlowRect(outer, COLOR_NEON_BLUE, 7, 0.025f, 4.0f);
    DrawRectangleRounded(outer, 0.025f, 12, FadeColorEx(COLOR_PANEL_FILL_1, 0.96f));
    DrawRectangleRounded(inner, 0.025f, 12, FadeColorEx(COLOR_PANEL_FILL_2, 0.98f));

    DrawCutCornerFrame(outer, 14.0f, 2.0f, COLOR_PANEL_LINE, 0.88f);
    DrawCutCornerFrame(inner, 10.0f, 1.0f, COLOR_NEON_CYAN, 0.28f);

    DrawTextEx(fontTitle, "QUANTUM", Vector2{ 108, 140 }, 42.0f, 2.0f, COLOR_TEXT_MAIN);
    DrawTextEx(fontTitle, "MODES", Vector2{ 128, 192 }, 42.0f, 2.0f, COLOR_TEXT_MAIN);

    DrawLineEx(Vector2{ 105, 265 }, Vector2{ 325, 265 }, 2.0f, FadeColorEx(COLOR_NEON_CYAN, 0.70f));
    DrawLineEx(Vector2{ 105, 272 }, Vector2{ 290, 272 }, 1.0f, FadeColorEx(COLOR_NEON_BLUE, 0.55f));

    DrawTextEx(fontTitle, "X  O  X", Vector2{ 132, 315 }, 28.0f, 4.0f, FadeColorEx(COLOR_NEON_CYAN, 0.95f));
    DrawTextEx(fontTitle, "O  X  O", Vector2{ 132, 356 }, 28.0f, 4.0f, FadeColorEx(COLOR_NEON_PINK, 0.92f));
    DrawTextEx(fontTitle, "X  X  O", Vector2{ 132, 397 }, 28.0f, 4.0f, FadeColorEx(COLOR_NEON_CYAN, 0.95f));

    DrawInfoCell(fontSmall, Rectangle{ 100, 462, 230, 34 }, "SYSTEM", "ONLINE", COLOR_NEON_CYAN);
    DrawInfoCell(fontSmall, Rectangle{ 100, 504, 230, 34 }, "USER", "GUEST-01", COLOR_NEON_BLUE);
    DrawInfoCell(fontSmall, Rectangle{ 100, 546, 230, 34 }, "GRID", "19 x 19", COLOR_NEON_PINK);
    DrawInfoCell(fontSmall, Rectangle{ 100, 588, 230, 34 }, "MODE", "TACTICAL", COLOR_NEON_CYAN);
}