#include "ui_save.h" 
#include "Model/Logic.h"
#include <cmath> 
#include <iostream>

// --- BIẾN TRẠNG THÁI HỆ THỐNG ---
static float gCursorBlinkTimer = 0.0f;
//static float gMessageTimer = 0.0f;          // Bộ đếm thời gian cho thông báo
//static const float MESSAGE_LIMIT = 3.0f;    // Thời gian hiển thị (3 giây)
//static bool gShouldExitAfterSave = false;   // Cờ tự động thoát sau khi lưu

// --- BIẾN DỮ LIỆU INPUT ---
static char gInputBuffer[32] = "";          // Buffer chứa tên save
static int gLetterCount = 0;                // Độ dài tên hiện tại
static const char* gStatusMsg = "";         // Nội dung thông báo

// --- CẤU HÌNH GIAO DIỆN ---
static constexpr float SAVE_PANEL_W = 600.0f;
static constexpr float SAVE_PANEL_H = 400.0f;

static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color)
{
    if (text == nullptr || text[0] == '\0') return;
    Vector2 sz = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(font, text,
        Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
        fontSize, 1.0f, color);
}

void InitSaveUI()
{
    gInputBuffer[0] = '\0';
    gLetterCount = 0;
    gStatusMsg = "";
    //gMessageTimer = 0.0f;
    //gShouldExitAfterSave = false;
    //gCursorBlinkTimer = 0.0f;
}

void ShutdownSaveUI()
{
    // Giải phóng tài nguyên nếu cần
}

void UpdateSaveUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    // 1. Cập nhật các bộ đếm thời gian
    gCursorBlinkTimer += dt;

    //if (gMessageTimer > 0)
    //{
    //    gMessageTimer -= dt;
    //    if (gMessageTimer <= 0)
    //    {
    //        gStatusMsg = ""; // Xóa chữ thông báo trên màn hình

    //        // Nếu lưu thành công thì tự động chuyển màn hình
    //        if (gShouldExitAfterSave)
    //        {
    //            currentScreen = SCREEN_PLAY;
    //            gShouldExitAfterSave = false;
    //        }
    //    }
    //}

    //// 2. Xử lý nhập liệu từ bàn phím (Chỉ cho phép khi chưa đang trong trạng thái chờ thoát)
    //if (!gShouldExitAfterSave)
    //{
    char key = GetCharPressed();
    while (key > 0)
    {
        if (key >= 32 && key <= 126 && gLetterCount < (int)sizeof(gInputBuffer) - 1)
        {
            gInputBuffer[gLetterCount] = key;
            gLetterCount++;
            gInputBuffer[gLetterCount] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && gLetterCount > 0)
    {
        gLetterCount--;
        gInputBuffer[gLetterCount] = '\0';
    }
    //}

    // 3. Xử lý logic các nút bấm
    for (int i = 0; i < gSaveButtonCount; ++i)
    {
        bool hovered = false, pressed = false;
        UpdateUIButton(50 + i, gSaveButtons[i], mouse, dt, audio, settings, hovered, pressed);

        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);

            if (gSaveButtons[i].id == SAVE_BTN_CONFIRM)
            {
                if (gLetterCount > 0)
                {
                    // LOGIC LƯU THÀNH CÔNG
                    strcpy_s(current().nameGame, sizeof(current().nameGame), gInputBuffer);
                    current().nameGame[sizeof(current().nameGame) - 1] = '\0'; // Đảm bảo null-terminated
                    gStatusMsg = "GAME SAVED SUCCESSFULLY!";
                    SaveData(current()); // Giả định hàm lưu của bạn nhận tên file

                    //gMessageTimer = MESSAGE_LIMIT; // Bắt đầu đếm ngược 3s
                    //gShouldExitAfterSave = true;   // Đặt lệnh chờ thoát
                }
                else
                {
                    // LOGIC LỖI
                    gStatusMsg = "NAME CANNOT BE EMPTY!";
                    //gMessageTimer = MESSAGE_LIMIT; // Hiện lỗi 3s rồi mất
                    //gShouldExitAfterSave = false;
                }
            }
            else if (gSaveButtons[i].id == SAVE_BTN_BACK)
            {
                currentScreen = SCREEN_PLAY;
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_PLAY;
}

void DrawSaveUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings)
{
    DrawBackgroundOnly();

    // Vẽ Panel chính
    Rectangle panel = {
        SCREEN_WIDTH * 0.5f - SAVE_PANEL_W * 0.5f,
        SCREEN_HEIGHT * 0.5f - SAVE_PANEL_H * 0.5f,
        SAVE_PANEL_W, SAVE_PANEL_H
    };
    DrawPanelFrame(panel);
	if (current().nameGame[0] == '\0') // Nếu chưa có tên game (lần đầu lưu), hiển thị giao diện nhập tên 
    {
        // 1. Tiêu đề
        DrawCenteredText(fontTitle, "SAVE GAME", panel.y + 35.0f, 40.0f, Color{ 255, 235, 225, 255 });

        // 2. Thông báo trạng thái (Hiện ra khi có gStatusMsg)
        if (gStatusMsg[0] != '\0')
        {
            // Màu đỏ cho lỗi (chữ N trong Name), màu xanh cho thành công
            Color msgColor = (gStatusMsg[0] == 'N') ? RED : LIME;
            DrawCenteredText(fontSmall, gStatusMsg, panel.y + 85.0f, 28.0f, msgColor);
        }

        // 3. Khung nhập liệu
        Rectangle inputBox = { panel.x + 100, panel.y + 130, 400, 50 };
        DrawRectangleRec(inputBox, Color{ 20, 20, 30, 255 });
        DrawRectangleLinesEx(inputBox, 2.0f, RAYWHITE);

        Vector2 textPos = { inputBox.x + 15, inputBox.y + 12 };
        if (gLetterCount == 0)
        {
            DrawTextEx(fontSmall, " Enter save name...", textPos, 24.0f, 1.0f, GRAY);
        }
        else
        {
            DrawTextEx(fontSmall, gInputBuffer, textPos, 24.0f, 1.0f, GOLD);
        }

        // 4. Con trỏ nhấp nháy (Cursor)
        if (fmodf(gCursorBlinkTimer, 1.0f) < 0.5f /* &&!gShouldExitAfterSave */)
        {
            Vector2 textSize = MeasureTextEx(fontSmall, gInputBuffer, 24.0f, 1.0f);
            DrawTextEx(fontSmall, "_", Vector2{ textPos.x + textSize.x + 2.0f, textPos.y }, 24.0f, 1.0f, GOLD);
        }

        // 5. Hướng dẫn
        DrawCenteredText(fontSmall, "Press CONFIRM to save your progress", panel.y + 200.0f, 20.0f, LIGHTGRAY);

        // 6. Vẽ các nút bấm
        for (int i = 0; i < gSaveButtonCount; ++i)
        {
            Rectangle hitRect = GetButtonRect(gSaveButtons[i]);
            bool hov = IsMouseOverRect(mouse, hitRect);
            bool prs = hov && mouse.leftDown;
            DrawUIButton(50 + i, gSaveButtons[i], fontSmall, hov, prs);
        }
    }
	else // Nếu đã có tên game (đã lưu ít nhất 1 lần), hiển thị giao diện thông báo lưu thành công 
    {
        DrawCenteredText(fontTitle, "GAME SAVED SUCCESSFULLY!", panel.y + 150.0f, 32.0f, LIME);
        // Hướng dẫn tiếp theo
        DrawCenteredText(fontSmall, "Press OK", panel.y + 220.0f, 20.0f, LIGHTGRAY);

        // Vẽ nút Back to Game
        Rectangle hitRect = GetButtonRect(gSaveButtons[1]); // Nút BACK
        bool hov = IsMouseOverRect(mouse, hitRect);
        bool prs = hov && mouse.leftDown;
        DrawUIButton(51, gSaveButtons[1], fontSmall, hov, prs);
    }
}

void UpdateSaveUISecond(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_PLAY;
    // Chỉ cần xử lý nút Back to Game
    Rectangle hitRect = GetButtonRect(gSaveButtons[1]); // Nút BACK
    bool hovered = IsMouseOverRect(mouse, hitRect);
    bool pressed = hovered && mouse.leftPressed;
    if (pressed)
    {
        PlayMenuClick(audio, settings);
        currentScreen = SCREEN_PLAY;
    }
}

//
void UpdateSaveAsUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    gCursorBlinkTimer += dt;

    //if (gMessageTimer > 0)
    //{
    //    gMessageTimer -= dt;
    //    if (gMessageTimer <= 0)
    //    {
    //        gStatusMsg = ""; // Xóa chữ thông báo trên màn hình

    //        // Nếu lưu thành công thì tự động chuyển màn hình
    //        if (gShouldExitAfterSave)
    //        {
    //            currentScreen = SCREEN_PLAY;
    //            gShouldExitAfterSave = false;
    //        }
    //    }
    //}

    //// 2. Xử lý nhập liệu từ bàn phím (Chỉ cho phép khi chưa đang trong trạng thái chờ thoát)
    //if (!gShouldExitAfterSave)
    //{

    char key = GetCharPressed();
    while (key > 0)
    {
        if (key >= 32 && key <= 126 && gLetterCount < (int)sizeof(gInputBuffer) - 1)
        {
            gInputBuffer[gLetterCount] = key;
            gLetterCount++;
            gInputBuffer[gLetterCount] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE) && gLetterCount > 0)
    {
        gLetterCount--;
        gInputBuffer[gLetterCount] = '\0';
    }
    //}

    // 3. Xử lý logic các nút bấm
    for (int i = 0; i < gSaveButtonCount; ++i)
    {
        bool hovered = false, pressed = false;
        UpdateUIButton(50 + i, gSaveButtons[i], mouse, dt, audio, settings, hovered, pressed);

        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);

            if (gSaveButtons[i].id == SAVE_BTN_CONFIRM)
            {
                if (gLetterCount > 0)
                {
                    // LOGIC LƯU THÀNH CÔNG

                    auto it = gameSaves.find(string(current().nameGame));
                    if (it != gameSaves.end()) {
                        gameSaves.erase(it); // Xóa tên cũ nếu đã tồn tại để tránh trùng lặp
                        strcpy_s(current().nameGame, sizeof(current().nameGame), gInputBuffer);
                        current().nameGame[sizeof(current().nameGame) - 1] = '\0'; // Đảm bảo null-terminated
                        gameSaves[string(current().nameGame)] = current();
                    }
                    gStatusMsg = "GAME SAVED SUCCESSFULLY WITH NEW NAME!";
                    SaveData(current()); // Giả định hàm lưu của bạn nhận tên file
                    SaveGamesToFile(gameSaves); // Cập nhật file saves.bin sau khi đổi tên

                    //gMessageTimer = MESSAGE_LIMIT; // Bắt đầu đếm ngược 3s
                    //gShouldExitAfterSave = true;   // Đặt lệnh chờ thoát
                }
                else
                {
                    SaveData(current()); // Dù tên trống vẫn lưu (với tên mặc định), nhưng sẽ hiển lỗi để người chơi biết
                    gStatusMsg = "GAME UPDATED.";
                    //gMessageTimer = MESSAGE_LIMIT; // Hiện lỗi 3s rồi mất
                    //gShouldExitAfterSave = false;
                }
            }
            else if (gSaveButtons[i].id == SAVE_BTN_BACK)
            {
                currentScreen = SCREEN_PLAY;
            }
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_PLAY;
}

void DrawSaveAsUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings)
{
    DrawBackgroundOnly();

    // Vẽ Panel chính
    Rectangle panel = {
        SCREEN_WIDTH * 0.5f - SAVE_PANEL_W * 0.5f,
        SCREEN_HEIGHT * 0.5f - SAVE_PANEL_H * 0.5f,
        SAVE_PANEL_W, SAVE_PANEL_H
    };
    DrawPanelFrame(panel);

    // 1. Tiêu đề
    DrawCenteredText(fontTitle, "SAVE GAME", panel.y + 35.0f, 40.0f, Color{ 255, 235, 225, 255 });

    // 2. Thông báo trạng thái (Hiện ra khi có gStatusMsg)
    if (gStatusMsg[0] != '\0')
    {
        // Màu đỏ cho lỗi (chữ N trong Name), màu xanh cho thành công
        Color msgColor = (gStatusMsg[0] == 'N') ? RED : LIME;
        DrawCenteredText(fontSmall, gStatusMsg, panel.y + 85.0f, 28.0f, msgColor);
    }

    // 3. Khung nhập liệu
    Rectangle inputBox = { panel.x + 100, panel.y + 130, 400, 50 };
    DrawRectangleRec(inputBox, Color{ 20, 20, 30, 255 });
    DrawRectangleLinesEx(inputBox, 2.0f, RAYWHITE);

    Vector2 textPos = { inputBox.x + 15, inputBox.y + 12 };
    if (gLetterCount == 0)
    {
        DrawTextEx(fontSmall, current().nameGame, textPos, 24.0f, 1.0f, GRAY);
    }
    else
    {
        DrawTextEx(fontSmall, gInputBuffer, textPos, 24.0f, 1.0f, GOLD);
    }

    // 4. Con trỏ nhấp nháy (Cursor)
    if (fmodf(gCursorBlinkTimer, 1.0f) < 0.5f /* &&!gShouldExitAfterSave */)
    {
        Vector2 textSize = MeasureTextEx(fontSmall, gInputBuffer, 24.0f, 1.0f);
        DrawTextEx(fontSmall, "_", Vector2{ textPos.x + textSize.x + 2.0f, textPos.y }, 24.0f, 1.0f, GOLD);
    }

    // 5. Hướng dẫn
    DrawCenteredText(fontSmall, "Press CONFIRM to save your progress", panel.y + 200.0f, 20.0f, LIGHTGRAY);

    // 6. Vẽ các nút bấm
    for (int i = 0; i < gSaveButtonCount; ++i)
    {
        Rectangle hitRect = GetButtonRect(gSaveButtons[i]);
        bool hov = IsMouseOverRect(mouse, hitRect);
        bool prs = hov && mouse.leftDown;
        DrawUIButton(50 + i, gSaveButtons[i], fontSmall, hov, prs);
    }
}

void UpdateSaveToBackMenuUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    if (current().nameGame[0] == '\0') // Nếu chưa có tên game (lần đầu lưu), hiển thị giao diện nhập tên
    {
        char key = GetCharPressed();
        while (key > 0)
        {
            if (key >= 32 && key <= 126 && gLetterCount < (int)sizeof(gInputBuffer) - 1)
            {
                gInputBuffer[gLetterCount] = key;
                gLetterCount++;
                gInputBuffer[gLetterCount] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && gLetterCount > 0)
        {
            gLetterCount--;
            gInputBuffer[gLetterCount] = '\0';
        }
        //}

        // 3. Xử lý logic các nút bấm
        for (int i = 0; i < gSaveButtonCount; ++i)
        {
            bool hovered = false, pressed = false;
            UpdateUIButton(50 + i, gSaveButtons[i], mouse, dt, audio, settings, hovered, pressed);

            if (hovered && mouse.leftPressed)
            {
                PlayMenuClick(audio, settings);

                if (gSaveButtons[i].id == SAVE_BTN_CONFIRM)
                {
                    if (gLetterCount > 0)
                    {
                        // LOGIC LƯU THÀNH CÔNG
                        strcpy_s(current().nameGame, sizeof(current().nameGame), gInputBuffer);
                        current().nameGame[sizeof(current().nameGame) - 1] = '\0'; // Đảm bảo null-terminated
                        gStatusMsg = "GAME SAVED SUCCESSFULLY! BACK TO MENU...";
                        SaveData(current()); // Giả định hàm lưu của bạn nhận tên file

                        InitNewGame();
                        currentScreen = SCREEN_MAIN_MENU; // Quay về menu chính sau khi lưu

                        //gMessageTimer = MESSAGE_LIMIT; // Bắt đầu đếm ngược 3s
                        //gShouldExitAfterSave = true;   // Đặt lệnh chờ thoát
                    }
                    else
                    {
                        // LOGIC LỖI
                        gStatusMsg = "NAME CANNOT BE EMPTY!";
                        //gMessageTimer = MESSAGE_LIMIT; // Hiện lỗi 3s rồi mất
                        //gShouldExitAfterSave = false;
                    }
                }
                else if (gSaveButtons[i].id == SAVE_BTN_BACK)
                {
                    currentScreen = SCREEN_PLAY;
                }
            }
        }
    }

    else {
        Rectangle hitRect = GetButtonRect(gSaveButtons[1]); // Nút BACK
        bool hovered = IsMouseOverRect(mouse, hitRect);
        bool pressed = hovered && mouse.leftPressed;
        if (pressed)
        {
            PlayMenuClick(audio, settings);
            currentScreen = SCREEN_MAIN_MENU;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_PLAY;
}

void UpdateSaveToExitUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
	ScreenState& currentScreen, 
    bool& shouldClose
)
{
    // Xử lý nút bấm
    if (current().nameGame[0] == '\0') // Nếu chưa có tên game (lần đầu lưu), hiển thị giao diện nhập tên
    {
        char key = GetCharPressed();
        while (key > 0)
        {
            if (key >= 32 && key <= 126 && gLetterCount < (int)sizeof(gInputBuffer) - 1)
            {
                gInputBuffer[gLetterCount] = key;
                gLetterCount++;
                gInputBuffer[gLetterCount] = '\0';
            }
            key = GetCharPressed();
        }

        if (IsKeyPressed(KEY_BACKSPACE) && gLetterCount > 0)
        {
            gLetterCount--;
            gInputBuffer[gLetterCount] = '\0';
        }
        //}

        // 3. Xử lý logic các nút bấm
        for (int i = 0; i < gSaveButtonCount; ++i)
        {
            bool hovered = false, pressed = false;
            UpdateUIButton(50 + i, gSaveButtons[i], mouse, dt, audio, settings, hovered, pressed);

            if (hovered && mouse.leftPressed)
            {
                PlayMenuClick(audio, settings);

                if (gSaveButtons[i].id == SAVE_BTN_CONFIRM)
                {
                    if (gLetterCount > 0)
                    {
                        // LOGIC LƯU THÀNH CÔNG
                        strcpy_s(current().nameGame, sizeof(current().nameGame), gInputBuffer);
                        current().nameGame[sizeof(current().nameGame) - 1] = '\0'; // Đảm bảo null-terminated
                        gStatusMsg = "GAME SAVED SUCCESSFULLY! BACK TO MENU...";
                        SaveData(current()); // Giả định hàm lưu của bạn nhận tên file 
                        SaveGamesToFile(gameSaves); // Cập nhật file saves.dat sau khi lưu
                        shouldClose = true; // Đặt cờ để thoát game sau khi lưu

                        //gMessageTimer = MESSAGE_LIMIT; // Bắt đầu đếm ngược 3s
                        //gShouldExitAfterSave = true;   // Đặt lệnh chờ thoát
                    }
                    else
                    {
                        // LOGIC LỖI
                        gStatusMsg = "NAME CANNOT BE EMPTY!";
                        //gMessageTimer = MESSAGE_LIMIT; // Hiện lỗi 3s rồi mất
                        //gShouldExitAfterSave = false;
                    }
                }
                else if (gSaveButtons[i].id == SAVE_BTN_BACK)
                {
                    currentScreen = SCREEN_PLAY;
                }
            }
        }
    }

    else {
        Rectangle hitRect = GetButtonRect(gSaveButtons[1]); // Nút BACK
        bool hovered = IsMouseOverRect(mouse, hitRect);
        bool pressed = hovered && mouse.leftPressed;
        if (pressed)
        {
            PlayMenuClick(audio, settings);
            shouldClose = true; // Đặt cờ để thoát game mà không lưu
		}
    }

    if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_PLAY;
}

void DrawSaveToExitUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings, bool isShouldClose)
{
    DrawBackgroundOnly();

    // Vẽ Panel chính
    Rectangle panel = {
        SCREEN_WIDTH * 0.5f - SAVE_PANEL_W * 0.5f,
        SCREEN_HEIGHT * 0.5f - SAVE_PANEL_H * 0.5f,
        SAVE_PANEL_W, SAVE_PANEL_H
    };
    DrawPanelFrame(panel);
    if (current().nameGame[0] == '\0') // Nếu chưa có tên game (lần đầu lưu), hiển thị giao diện nhập tên 
    {
        // 1. Tiêu đề
        DrawCenteredText(fontTitle, "SAVE GAME", panel.y + 35.0f, 40.0f, Color{ 255, 235, 225, 255 });

        // 2. Thông báo trạng thái (Hiện ra khi có gStatusMsg)
        if (gStatusMsg[0] != '\0')
        {
            // Màu đỏ cho lỗi (chữ N trong Name), màu xanh cho thành công
            Color msgColor = (gStatusMsg[0] == 'N') ? RED : LIME;
            DrawCenteredText(fontSmall, gStatusMsg, panel.y + 85.0f, 28.0f, msgColor);
        }

        // 3. Khung nhập liệu
        Rectangle inputBox = { panel.x + 100, panel.y + 130, 400, 50 };
        DrawRectangleRec(inputBox, Color{ 20, 20, 30, 255 });
        DrawRectangleLinesEx(inputBox, 2.0f, RAYWHITE);

        Vector2 textPos = { inputBox.x + 15, inputBox.y + 12 };
        if (gLetterCount == 0)
        {
            DrawTextEx(fontSmall, " Enter save name...", textPos, 24.0f, 1.0f, GRAY);
        }
        else
        {
            DrawTextEx(fontSmall, gInputBuffer, textPos, 24.0f, 1.0f, GOLD);
        }

        // 4. Con trỏ nhấp nháy (Cursor)
        if (fmodf(gCursorBlinkTimer, 1.0f) < 0.5f /* &&!gShouldExitAfterSave */)
        {
            Vector2 textSize = MeasureTextEx(fontSmall, gInputBuffer, 24.0f, 1.0f);
            DrawTextEx(fontSmall, "_", Vector2{ textPos.x + textSize.x + 2.0f, textPos.y }, 24.0f, 1.0f, GOLD);
        }

        // 5. Hướng dẫn
        DrawCenteredText(fontSmall, "Press CONFIRM to save your progress", panel.y + 200.0f, 20.0f, LIGHTGRAY);

        // 6. Vẽ các nút bấm
        for (int i = 0; i < gSaveButtonCount; ++i)
        {
            Rectangle hitRect = GetButtonRect(gSaveButtons[i]);
            bool hov = IsMouseOverRect(mouse, hitRect);
            bool prs = hov && mouse.leftDown;
            DrawUIButton(50 + i, gSaveButtons[i], fontSmall, hov, prs);
        }
    }
    else // Nếu đã có tên game (đã lưu ít nhất 1 lần), hiển thị giao diện thông báo lưu thành công 
    {
        DrawCenteredText(fontTitle, "GAME SAVED SUCCESSFULLY!", panel.y + 150.0f, 32.0f, LIME);
        // Hướng dẫn tiếp theo
        DrawCenteredText(fontSmall, "Press OK", panel.y + 220.0f, 20.0f, LIGHTGRAY);

        // Vẽ nút Back to Game
        Rectangle hitRect = GetButtonRect(gSaveButtons[1]); // Nút BACK
        bool hov = IsMouseOverRect(mouse, hitRect);
        bool prs = hov && mouse.leftDown;
        DrawUIButton(51, gSaveButtons[1], fontSmall, hov, prs);
    }
}