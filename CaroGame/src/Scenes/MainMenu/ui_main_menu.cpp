#include "Scenes/MainMenu/ui_main_menu.h"
#include "View/ui_background.h"
#include "View/ui_button.h"

void InitMainMenuUI()
{
    InitUIButtonSystem();
}

void ShutdownMainMenuUI()
{
    ShutdownUIButtonSystem();
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
                currentScreen = SCREEN_SETUP;
                break;

            case BTN_ABOUT:
                currentScreen = SCREEN_ABOUT;
                break;

            case BTN_SETTING:
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
    (void)fontSmall;

    DrawBackgroundOnly();
    DrawLogoOnly();

    for (int i = 0; i < gMainMenuButtonCount; ++i)
    {
        const Rectangle hitRect = GetButtonRect(gMainMenuButtons[i]);
        const bool hovered = IsMouseOverRect(mouse, hitRect);
        const bool pressed = hovered && mouse.leftDown;

        DrawUIButton(i, gMainMenuButtons[i], fontTitle, hovered, pressed);
    }
}