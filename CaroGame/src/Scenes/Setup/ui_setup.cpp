#include "Scenes/Setup/ui_setup.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "Model/config.h"

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

void InitSetupUI()
{
}

void ShutdownSetupUI()
{
}

void UpdateSetupUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    for (int i = 0; i < gSetupButtonCount; ++i)
    {
        const int animIndex = 10 + i;

        bool hovered = false;
        bool pressed = false;

        UpdateUIButton(
            animIndex,
            gSetupButtons[i],
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

            switch (gSetupButtons[i].id)
            {
            case SETUP_BTN_PLAY:
                currentScreen = SCREEN_PLAY;
                break;

            case SETUP_BTN_BACK:
                currentScreen = SCREEN_MAIN_MENU;
                break;
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        currentScreen = SCREEN_MAIN_MENU;
    }
}

void DrawSetupUI(Font fontTitle, Font fontSmall, const MouseState& mouse)
{
    DrawBackgroundOnly();
    DrawLogoOnly();

    DrawCenteredText(fontSmall, "SETUP SCREEN PLACEHOLDER", 610.0f, 24.0f, Color{ 235, 225, 230, 210 });

    for (int i = 0; i < gSetupButtonCount; ++i)
    {
        const int animIndex = 10 + i;

        Rectangle hitRect = GetButtonRect(gSetupButtons[i]);
        bool hovered = IsMouseOverRect(mouse, hitRect);
        bool pressed = hovered && mouse.leftDown;

        DrawUIButton(animIndex, gSetupButtons[i], fontTitle, hovered, pressed);
    }

    DrawCenteredText(fontSmall, "PRESS ESC TO BACK", 850.0f, 20.0f, Color{ 220, 205, 205, 180 });
}