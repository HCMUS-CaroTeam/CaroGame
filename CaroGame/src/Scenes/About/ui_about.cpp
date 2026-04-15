#include "Scenes/About/ui_about.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/config.h"

// ════════════════════════════════════════════════════════════════════════
//  NỘI DUNG MÀN ABOUT — chỉnh tại đây
// ════════════════════════════════════════════════════════════════════════

// ── PANEL ────────────────────────────────────────────────────────────────
// Chỉnh ABOUT_PANEL_X/Y → dịch panel trái/phải/lên/xuống
// Chỉnh ABOUT_PANEL_W/H → thay đổi kích thước panel
static constexpr float ABOUT_PANEL_X = SCREEN_WIDTH  * 0.5f - 480.0f;  // ← vị trí X panel
static constexpr float ABOUT_PANEL_Y = 230.0f;                          // ← vị trí Y panel
static constexpr float ABOUT_PANEL_W = 960.0f;                          // ← độ rộng panel
static constexpr float ABOUT_PANEL_H = 440.0f;                          // ← độ cao panel

// ── NỘI DUNG VĂN BẢN ─────────────────────────────────────────────────
// Thêm/bớt/sửa các dòng text tại đây
// Mỗi phần tử = 1 dòng hiển thị trên màn hình
static const char* ABOUT_LINES[] = {
    "QUANTUM CARO",
    "",
    "A pixel-art Caro / Gomoku game built with raylib.",
    "",
    "Place 5 pieces in a row to win.",
    "Supports 2-player local play.",
    "",
    "Made with  raylib  +  C++",
};
static constexpr int ABOUT_LINE_COUNT = sizeof(ABOUT_LINES) / sizeof(ABOUT_LINES[0]);

// ── VỊ TRÍ TEXT ──────────────────────────────────────────────────────
// ABOUT_TEXT_START_Y : Y bắt đầu của dòng đầu tiên
// ABOUT_LINE_SPACING : khoảng cách giữa các dòng (px)
static constexpr float ABOUT_TEXT_START_Y = 290.0f;   // ← Y dòng đầu tiên
static constexpr float ABOUT_LINE_SPACING = 38.0f;    // ← khoảng cách giữa các dòng

// ════════════════════════════════════════════════════════════════════════

static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color)
{
    if (!text || text[0] == '\0') return;
    Vector2 sz = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(font, text,
        Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
        fontSize, 1.0f, color);
}

// ── Public API ────────────────────────────────────────────────────────
void InitAboutUI()  {}
void ShutdownAboutUI() {}

void UpdateAboutUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    // Nút BACK (animIndex = 31)
    bool hovered = false, pressed = false;
    UpdateUIButton(31, gAboutButtons[0], mouse, dt, audio, settings, hovered, pressed);

    if (hovered && mouse.leftPressed)
    {
        PlayMenuClick(audio, settings);
        currentScreen = SCREEN_MAIN_MENU;
    }

    if (IsKeyPressed(KEY_ESCAPE))
        currentScreen = SCREEN_MAIN_MENU;
}

void DrawAboutUI(
    Font fontTitle,
    Font fontSmall,
    const MouseState& mouse
)
{
    DrawBackgroundOnly();
    DrawLogoOnly();

    // Panel
    Rectangle panel = { ABOUT_PANEL_X, ABOUT_PANEL_Y, ABOUT_PANEL_W, ABOUT_PANEL_H };
    DrawPanelFrame(panel);

    // Các dòng nội dung
    float y = ABOUT_TEXT_START_Y;
    for (int i = 0; i < ABOUT_LINE_COUNT; ++i)
    {
        if (i == 0)
        {
            // Dòng đầu dùng font title, to hơn
            DrawCenteredText(fontTitle, ABOUT_LINES[i], y, 36.0f, Color{ 255, 235, 225, 255 });
            y += ABOUT_LINE_SPACING + 8.0f;
        }
        else
        {
            DrawCenteredText(fontSmall, ABOUT_LINES[i], y, (float)fontSmall.baseSize, Color{ 220, 210, 230, 210 });
            y += ABOUT_LINE_SPACING;
        }
    }

    // Nút BACK (animIndex = 31)
    Rectangle hitRect = GetButtonRect(gAboutButtons[0]);
    bool hov = IsMouseOverRect(mouse, hitRect);
    bool prs = hov && mouse.leftDown;
    DrawUIButton(31, gAboutButtons[0], fontSmall, hov, prs);
}
