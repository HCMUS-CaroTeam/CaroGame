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
    AppSettings& settings,
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
            case SETUP_BTN_PVE: // Người chơi bấm chuyển tab PVE
                settings.gameMode = 1;
                break;

            case SETUP_BTN_PVP: // Người chơi bấm chuyển tab PVP
                settings.gameMode = 2;
                break;

            case SETUP_BTN_PREV: // NÚT MŨI TÊN TRÁI (<)
                if (settings.gameMode == 1) {
                    // Đang ở tab PVE -> Xoay vòng Boss (1 đến 3)
                    settings.botDifficulty--;
                    if (settings.botDifficulty < 1) settings.botDifficulty = 3;
                }
                else if (settings.gameMode == 2) {
                    // Đang ở tab PVP -> Xoay vòng chế độ PvP (1 đến 2)
                    settings.pvpMode--;
                    if (settings.pvpMode < 1) settings.pvpMode = 2;
                }
                break;

            case SETUP_BTN_NEXT: // NÚT MŨI TÊN PHẢI (>)
                if (settings.gameMode == 1) {
                    settings.botDifficulty++;
                    if (settings.botDifficulty > 3) settings.botDifficulty = 1;
                }
                else if (settings.gameMode == 2) {
                    settings.pvpMode++;
                    if (settings.pvpMode > 2) settings.pvpMode = 1;
                }
                break;

            case SETUP_BTN_PLAY:
                currentScreen = SCREEN_PLAY; // Xong xuôi thì bay vào game
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

void DrawSetupUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings)
{
    DrawBackgroundOnly();
    DrawLogoOnly();

    // --- BẮT ĐẦU VẼ CAROUSEL ---
    const char* carouselText = "";
    Color textColor = WHITE;

    if (settings.gameMode == 1)
    {
        // CHẾ ĐỘ PVE
        if (settings.botDifficulty == 1) carouselText = "BOSS 1: KE TAU HAI";
        else if (settings.botDifficulty == 2) carouselText = "BOSS 2: KE HUY DIET";
        else if (settings.botDifficulty == 3) carouselText = "BOSS 3: CHUA TE CARO";

        textColor = GREEN;
    }
    else if (settings.gameMode == 2)
    {
        // CHẾ ĐỘ PVP
        if (settings.pvpMode == 1) carouselText = "MODE: CLASSIC (TRUYEN THONG)";
        else if (settings.pvpMode == 2) carouselText = "MODE: TOURNAMENT (CHAN 2 DAU)";

        textColor = RED;
    }

    // Đẩy chữ ra giữa màn hình
    DrawCenteredText(fontSmall, carouselText, 610.0f, 30.0f, textColor);
    // --- KẾT THÚC VẼ CAROUSEL ---

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