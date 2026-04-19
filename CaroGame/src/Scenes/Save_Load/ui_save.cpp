//#include "ui_save.h" // Giả định bạn sẽ tạo file header này
//
//
//// Biến tĩnh nội bộ cho màn hình Save
//static char gInputBuffer[32] = "NewSave"; // Buffer chứa tên save
//static int gLetterCount = 0;               // Độ dài tên hiện tại
//static const char* gStatusMsg = "";        // Thông báo trạng thái (Lưu thành công/thất bại)
//
//Button gSaveButtons[] =
//{
//    {
//        Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 480.0f }, // Vị trí X, Y
//        Vector2{ 320.0f, 60.0f },                        // Rộng, Cao
//        "CONFIRM SAVE",                                  // Chữ trên nút
//        SAVE_BTN_CONFIRM,                                // ID
//        BUTTON_VISUAL_TEXT,
//        BUTTON_ICON_NONE,
//        24.0f, 1.5f
//    },
//    {
//        Vector2{ SCREEN_WIDTH * 0.5f - 160.0f, 560.0f },
//        Vector2{ 320.0f, 60.0f },
//        "BACK TO GAME",
//        SAVE_BTN_BACK,
//        BUTTON_VISUAL_TEXT,
//        BUTTON_ICON_NONE,
//        24.0f, 1.5f
//    }
//};
//
//int gSaveButtonCount = 2;
//
//// Cấu hình vị trí Panel (tương tự About UI của bạn)
//static constexpr float SAVE_PANEL_W = 600.0f;
//static constexpr float SAVE_PANEL_H = 400.0f;
//
//
//static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color)
//{
//    if (!text || text[0] == '\0') return;
//    Vector2 sz = MeasureTextEx(font, text, fontSize, 1.0f);
//    DrawTextEx(font, text,
//        Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
//        fontSize, 1.0f, color);
//}
//
//void InitSaveUI()
//{
//    // Reset buffer mỗi khi mở màn hình
//    memset(gInputBuffer, 0, sizeof(gInputBuffer));
//	gInputBuffer[0] = '\0';
//    gLetterCount = (int)strlen(gInputBuffer);
//    gStatusMsg = "";
//}
//
//void ShutdownSaveUI() {}
//
//void UpdateSaveUI(
//    const MouseState& mouse,
//    float dt,
//    AudioAssets& audio,
//    const AppSettings& settings,
//    ScreenState& currentScreen
//)
//{
//    // 1. Xử lý nhập tên từ bàn phím (Cơ bản)
//    int key = GetCharPressed();
//    while (key > 0)
//    {
//        if ((key >= 32) && (key <= 125) && (gLetterCount < 31))
//        {
//            gInputBuffer[gLetterCount] = (char)key;
//            gInputBuffer[gLetterCount + 1] = '\0';
//            gLetterCount++;
//        }
//        key = GetCharPressed();
//    }
//
//    if (IsKeyPressed(KEY_BACKSPACE) && gLetterCount > 0)
//    {
//        gLetterCount--;
//        gInputBuffer[gLetterCount] = '\0';
//    }
//
//    // 2. Cập nhật nút SAVE và BACK
//    // Giả định bạn có gSaveButtons trong menu_data (giống gAboutButtons)
//    // Ở đây tôi dùng index 40 cho Save và 41 cho Back để tránh trùng với Play UI (20+)
//
//    for (int i = 0; i < gSaveButtonCount; ++i)
//    {
//        bool hovered = false, pressed = false;
//        UpdateUIButton(40 + i, gSaveButtons[i], mouse, dt, audio, settings, hovered, pressed);
//
//        if (hovered && mouse.leftPressed)
//        {
//            PlayMenuClick(audio, settings);
//
//            if (gSaveButtons[i].id == SAVE_BTN_CONFIRM) // Bạn cần định nghĩa ID này
//            {
//                // Lấy dữ liệu hiện tại và đặt tên
//                DataGame& currentData = GetCurrentGameData();
//                strcpy_s(currentData.nameGame, sizeof(currentData.nameGame), gInputBuffer);
//
//                // Gọi logic lưu trong save.cpp
//                SaveGame(currentData);
//                SaveToFile(savedGames()); // Lưu xuống file vật lý
//
//                gStatusMsg = "GAME SAVED SUCCESSFULLY!";
//            }
//            else if (gSaveButtons[i].id == SAVE_BTN_BACK)
//            {
//                currentScreen = SCREEN_PLAY; // Quay lại màn hình chơi
//            }
//        }
//    }
//
//    if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_PLAY;
//}
//
//void DrawSaveUI(Font fontTitle, Font fontSmall, const MouseState& mouse)
//{
//    DrawBackgroundOnly();
//
//    Rectangle panel = {
//        SCREEN_WIDTH * 0.5f - SAVE_PANEL_W * 0.5f,
//        SCREEN_HEIGHT * 0.5f - SAVE_PANEL_H * 0.5f,
//        SAVE_PANEL_W, SAVE_PANEL_H
//    };
//    DrawPanelFrame(panel);
//
//    DrawCenteredText(fontTitle, "SAVE GAME", panel.y + 40.0f, 40.0f, Color{ 255, 235, 225, 255 });
//
//    // Vẽ hộp nhập liệu (Input Box)
//    Rectangle inputBox = { panel.x + 100, panel.y + 120, 400, 50 };
//    DrawRectangleRec(inputBox, Color{ 30, 30, 45, 255 });
//    DrawRectangleLinesEx(inputBox, 2.0f, RAYWHITE);
//
//    DrawTextEx(fontSmall, gInputBuffer, Vector2{ inputBox.x + 10, inputBox.y + 15 }, 24.0f, 1.0f, GOLD);
//
//    // Vẽ hướng dẫn
//    DrawCenteredText(fontSmall, "Type a name and press SAVE", panel.y + 180.0f, 20.0f, GRAY);
//
//    // Vẽ thông báo trạng thái
//    if (gStatusMsg[0] != '\0')
//    {
//        DrawCenteredText(fontSmall, gStatusMsg, panel.y + 220.0f, 22.0f, LIME);
//    }
//
//    // Vẽ các nút (Save & Back)
//    for (int i = 0; i < gSaveButtonCount; ++i)
//    {
//        Rectangle hitRect = GetButtonRect(gSaveButtons[i]);
//        bool hov = IsMouseOverRect(mouse, hitRect);
//        bool prs = hov && mouse.leftDown;
//        DrawUIButton(40 + i, gSaveButtons[i], fontSmall, hov, prs);
//    }
//}