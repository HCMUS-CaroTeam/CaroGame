#include "Scenes/Play/ui_pause.h"
#include "View/ui_button.h"
#include "Model/config.h"
#include "Scenes/Save_Load/ui_save.h"
#include "Scenes/Save_Load/ui_load.h"

static bool gPaused = false;
static const char* gPauseMessage = "";

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

static Rectangle GetPausePanelRect()
{
    return Rectangle{
        SCREEN_WIDTH * 0.5f - 240.0f,
        SCREEN_HEIGHT * 0.5f - 400.0f,
        480.0f,
        800.0f
    };
}

void InitPauseUI()
{
    gPaused = false;
    gPauseMessage = "";
}

void ShutdownPauseUI()
{
}

void TogglePause()
{
    gPaused = !gPaused;
    gPauseMessage = "";
}

void ClosePause()
{
    gPaused = false;
    gPauseMessage = "";
}

bool IsPauseActive()
{
    return gPaused;
}

void UpdatePauseUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
)
{
    if (!gPaused)
    {
        return;
    }

    for (int i = 0; i < gPauseButtonCount; ++i)
    {
        const int animIndex = 20 + i;

        bool hovered = false;
        bool pressed = false;

        UpdateUIButton(
            animIndex,
            gPauseButtons[i],
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

            switch (gPauseButtons[i].id)
            {
            case PAUSE_BTN_CONTINUE:
                ClosePause();
                break;

            case PAUSE_BTN_SETTING:
                ClosePause();
                currentScreen = SCREEN_SETTING;
                break;

            case PAUSE_BTN_SAVE:
                ClosePause();
                if (current().nameGame[0] == '\0') {
                    currentScreen = SCREEN_SAVE_FIRST;
                }
                else {
                    currentScreen = SCREEN_SAVE_SECOND;
                }
                break;

            case PAUSE_BTN_SAVE_AS:
                ClosePause();
                if (current().nameGame[0] == '\0') {
                    currentScreen = SCREEN_SAVE_FIRST;
                }
                else {
                    InitSaveUI();
                    currentScreen = SCREEN_SAVE_AS;
                }
                break;

            case PAUSE_BTN_EXIT_MENU:
                ClosePause();
                currentScreen = SCREEN_NOTIFY_BACK_MENU;
                break;

            case PAUSE_BTN_EXIT_DESKTOP:
                ClosePause();
				currentScreen = SCREEN_NOTIFY_EXIT;
                break;
            }
        }
    }
}

void DrawPauseUI(Font fontTitle, Font fontSmall, const MouseState& mouse)
{
    if (!gPaused)
    {
        return;
    }

    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 150 });

    Rectangle panel = GetPausePanelRect();

    DrawRectangleRounded(panel, 0.08f, 12, Color{ 20, 16, 35, 235 });
    DrawRectangleRoundedLinesEx(panel, 0.08f, 12, 3.0f, Color{ 220, 230, 255, 220 });

    DrawCenteredText(fontTitle, "PAUSE", panel.y + 32.0f, 36.0f, Color{ 255, 235, 225, 255 });

    for (int i = 0; i < gPauseButtonCount; ++i)
    {
        const int animIndex = 20 + i;
        Rectangle hitRect = GetButtonRect(gPauseButtons[i]);
        bool hovered = IsMouseOverRect(mouse, hitRect);
        bool pressed = hovered && mouse.leftDown;

        DrawUIButton(animIndex, gPauseButtons[i], fontSmall, hovered, pressed);
    }

    if (gPauseMessage && gPauseMessage[0] != '\0')
    {
        DrawCenteredText(
            fontSmall,
            gPauseMessage,
            panel.y + panel.height - 44.0f,
            20.0f,
            Color{ 255, 210, 210, 220 }
        );
    }
}