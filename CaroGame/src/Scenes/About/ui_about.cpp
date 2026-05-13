#include "Scenes/About/ui_about.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/config.h"

static constexpr float ABOUT_PANEL_X = SCREEN_WIDTH * 0.5f - 450.0f;
static constexpr float ABOUT_PANEL_Y = 180.0f;
static constexpr float ABOUT_PANEL_W = 900.0f;
static constexpr float ABOUT_PANEL_H = 520.0f;

static constexpr float ABOUT_BADGE_W = 230.0f;
static constexpr float ABOUT_BADGE_H = 68.0f;
static constexpr float ABOUT_BADGE_Y = ABOUT_PANEL_Y + 50.0f;

static const char* ABOUT_LINES[] = {
    "University of Science - VNUHCM",
    "CARO GAME PROJECT",
    "Game developed by:",
    "24120085 - Lê Nguyễn Thùy Linh",
    "24120370 - Trần Thị Lợi",
    "24120471 - Nguyễn Trần Minh Trí",
    "24120454 - Huỳnh Trần Phước Thiện",
    "24120468 - Âu Bảo Trân",
};
static constexpr int ABOUT_LINE_COUNT = sizeof(ABOUT_LINES) / sizeof(ABOUT_LINES[0]);

static constexpr float ABOUT_LINE_Y[] = {
    ABOUT_PANEL_Y + 135.0f,
    ABOUT_PANEL_Y + 187.0f,
    ABOUT_PANEL_Y + 266.0f,
    ABOUT_PANEL_Y + 304.0f,
    ABOUT_PANEL_Y + 342.0f,
    ABOUT_PANEL_Y + 380.0f,
    ABOUT_PANEL_Y + 418.0f,
    ABOUT_PANEL_Y + 456.0f,
};

static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color, float spacing = 2.0f)
{
    if (!text || text[0] == '\0') return;

    Vector2 sz = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextEx(
        font,
        text,
        Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
        fontSize,
        spacing,
        color
    );
}

void InitAboutUI() {}
void ShutdownAboutUI() {}

void UpdateAboutUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
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

    Rectangle panel = { ABOUT_PANEL_X, ABOUT_PANEL_Y, ABOUT_PANEL_W, ABOUT_PANEL_H };
    DrawPanelFrame(panel);

    Button badgeButton = {
        Vector2{ SCREEN_WIDTH * 0.5f - ABOUT_BADGE_W * 0.5f, ABOUT_BADGE_Y },
        Vector2{ ABOUT_BADGE_W, ABOUT_BADGE_H },
        "ABOUT US",
        ABOUT_BTN_BACK,
        BUTTON_VISUAL_TEXT,
        BUTTON_ICON_NONE,
        26.0f,
        1.0f
    };

    DrawUIButton(29, badgeButton, fontTitle, false, false);

    for (int i = 0; i < ABOUT_LINE_COUNT; ++i)
    {
        const bool isTopTitle = i == 0 || i == 1;
        const bool isSectionTitle = i == 2;
        Font textFont = isTopTitle ? fontTitle : fontSmall;
        const float fontSize = isTopTitle ? 36.0f : (isSectionTitle ? 30.0f : 28.0f);
        DrawCenteredText(textFont, ABOUT_LINES[i], ABOUT_LINE_Y[i], fontSize, Color{ 255, 255, 255, 255 });
    }

    Rectangle hitRect = GetButtonRect(gAboutButtons[0]);
    bool hov = IsMouseOverRect(mouse, hitRect);
    bool prs = hov && mouse.leftDown;
    DrawUIButton(31, gAboutButtons[0], fontTitle, hov, prs);
}
