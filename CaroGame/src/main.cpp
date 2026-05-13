#include "raylib.h"
#include "Model/config.h"
#include "Control/input_mouse.h"
#include "Scenes/MainMenu/ui_main_menu.h"
#include "View/ui_background.h"
#include "View/ui_frame.h"
#include "Systems/audio_manager.h"
#include "Model/app_settings.h"
#include "Control/menu_data.h"
#include "Scenes/Setup/ui_setup.h"
#include "Scenes/Play/ui_play.h"
#include "Scenes/Setting/ui_setting.h"
#include "Scenes/About/ui_about.h"
#include "View/ui_button.h"
#include "Scenes/Save_Load/ui_save.h"
#include "Scenes/Save_Load/ui_load.h"
#include "Scenes/Notify/ui_notify.h"

static int* GetVietnameseCodepoints(int& codepointCount)
{
    static int codepoints[95 + 64 + 128 + 90] = {};
    static bool initialized = false;

    if (!initialized)
    {
        int index = 0;

        for (int c = 32; c <= 126; ++c)
            codepoints[index++] = c;

        for (int c = 0x00C0; c <= 0x00FF; ++c)
            codepoints[index++] = c;

        for (int c = 0x0100; c <= 0x017F; ++c)
            codepoints[index++] = c;

        for (int c = 0x1EA0; c <= 0x1EF9; ++c)
            codepoints[index++] = c;

        initialized = true;
    }

    codepointCount = sizeof(codepoints) / sizeof(codepoints[0]);
    return codepoints;
}

static Font LoadFontSafe(const char* path, int size)
{
    if (FileExists(path))
    {
        int codepointCount = 0;
        int* codepoints = GetVietnameseCodepoints(codepointCount);
        return LoadFontEx(path, size, codepoints, codepointCount);
    }

    return GetFontDefault();
}


int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Quantum Caro");
    SetExitKey(KEY_NULL);
    SetTargetFPS(TARGET_FPS);

    Font fontTitle = LoadFontSafe(FONT_PATH, 64);
    Font fontSmall = LoadFontSafe(FONT_PATH, 23);
    Font fontMini = LoadFontSafe(FONT_PATH, 16);

    AppSettings settings{};
    LoadSettings(settings);

    AudioAssets audio{};

    InitGameAudio(audio);
    InitUIButtonSystem();
    InitUIFrameSystem();
    InitMainMenuUI();
    InitSetupUI();
    InitPlayUI();
    InitSettingUI();
    InitAboutUI();
    InitSaveUI();
    InitLoadUI();
	InitNotifyUI();
    ScreenState currentScreen = SCREEN_MAIN_MENU;
    bool shouldClose = false;

    while (!WindowShouldClose() && !shouldClose)
    {
        const MouseState mouse = GetMouseStateNow();
        const float dt = GetFrameTime();

        UpdateGameAudio(audio, settings);

        switch (currentScreen)
        {
        case SCREEN_MAIN_MENU:
            UpdateMainMenuUI(mouse, dt, audio, settings, currentScreen, shouldClose);
            break;

        case SCREEN_SETUP:
            UpdateSetupUI(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_PLAY:
            UpdatePlayUI(mouse, dt, audio, settings, currentScreen, shouldClose);
            break;

        case SCREEN_SETTING:
            UpdateSettingUI(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_ABOUT:
            UpdateAboutUI(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_SAVE_FIRST:
            UpdateSaveUI(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_SAVE_SECOND:
            UpdateSaveUISecond(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_SAVE_AS:
            UpdateSaveAsUI(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_LOAD:
            UpdateLoadUI(mouse, dt, audio, settings, currentScreen);
            break;

        case SCREEN_NOTIFY_EXIT:
            UpdateNotifyUI(mouse, dt, audio, settings, currentScreen, shouldClose, true);
			break;

        case SCREEN_NOTIFY_BACK_MENU:
			UpdateNotifyUI(mouse, dt, audio, settings, currentScreen, shouldClose, false);
			break;

		case SCREEN_SAVE_TO_BACK_MENU:
			UpdateSaveToBackMenuUI(mouse, dt, audio, settings, currentScreen);
			break;

		case SCREEN_SAVE_TO_EXIT:
			UpdateSaveToExitUI(mouse, dt, audio, settings, currentScreen, shouldClose);
			break;
        }

        BeginDrawing();

        switch (currentScreen)
        {
        case SCREEN_MAIN_MENU:
            DrawMainMenuUI(fontTitle, fontSmall, mouse);
            break;

        case SCREEN_SETUP:
            DrawSetupUI(fontTitle, fontSmall, mouse, settings);
            break;

        case SCREEN_PLAY:
            DrawPlayUI(fontTitle, fontSmall, mouse, settings);
            break;

        case SCREEN_ABOUT:
            DrawAboutUI(fontTitle, fontSmall, mouse);
            break;

        case SCREEN_SETTING:
            DrawSettingUI(fontTitle, fontSmall, fontMini, mouse, settings);
            break;

		case SCREEN_SAVE_FIRST: 
        case SCREEN_SAVE_SECOND:       
        case SCREEN_SAVE_TO_BACK_MENU:
        case SCREEN_SAVE_TO_EXIT:
			DrawSaveUI(fontTitle, fontSmall, mouse, settings);
            break;

        case SCREEN_SAVE_AS:
            DrawSaveAsUI(fontTitle, fontSmall, mouse, settings);
            break;

        case SCREEN_LOAD:
            DrawLoadUI(fontTitle, fontSmall, mouse, settings);
            break;

		case SCREEN_NOTIFY_EXIT: 
        case SCREEN_NOTIFY_BACK_MENU:
			DrawNotifyUI(fontTitle, fontSmall, mouse, settings);
        }

        // UI brightness overlay (darkens the scene; 1.0=brightest, 0.0=very dark)
        if (settings.uiBrightness < 0.999f)
        {
            unsigned char alpha = (unsigned char)((1.0f - settings.uiBrightness) * 200.0f);
            DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, alpha });
        }

        EndDrawing();
    }

    SaveGamesToFile(gameSaves);

    SaveSettings(settings);
    ShutdownAboutUI();
    ShutdownSettingUI();
    ShutdownPlayUI();
    ShutdownSetupUI();
    ShutdownAboutUI();
    ShutdownSaveUI();
    ShutdownLoadUI();
    ShutdownMainMenuUI();
    ShutdownNotifyUI();
    ShutdownUIFrameSystem();
    ShutdownUIButtonSystem();
    ShutdownGameAudio(audio);
    UnloadBackgroundAssets();

    if (FileExists(FONT_PATH))
    {
        UnloadFont(fontTitle);
        UnloadFont(fontSmall);
        UnloadFont(fontMini);
    }

    CloseWindow();
    return 0;
}
