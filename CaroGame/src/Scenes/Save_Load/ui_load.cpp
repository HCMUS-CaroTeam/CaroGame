#include"ui_load.h"
#include< string>
using namespace std;

static float gScrollY = 0.0f;
static string gSelectedKey = "";
static const float ITEM_HEIGHT = 120.0f;
static const float VIEW_HEIGHT = 450.0f;

// Trạng thái cho Rename
static bool gShowRenameOverlay = false;
static char gRenameBuffer[32] = "";
static int gRenameLetterCount = 0;
static float gCursorBlinkTimer = 0.0f;
static const char* gRenameStatusMsg = "";

// Trạng thái cho Delete
static bool gShowDeleteConfirm = false;

// Trạng thái cho Notification chung (dùng cho cả Rename và Delete)
static bool gShowNotification = false;
static string gNotificationMessageLine1 = "";
static string gNotificationMessageLine2 = "";

// Hàm hỗ trợ
static const bool IsValidChar(char key)
{
    return (isalnum(key) || key == '_');
}

// Nút Rename, Delete và OK 
static Button gRenameButtons[] = {
    { { SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30 }, { 140.0f , 45.0f }, "CONFIRM", 100, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { { SCREEN_WIDTH / 2 + 10 , SCREEN_HEIGHT / 2 + 30 }, { 140.0f , 45.0f }, "CANCEL", 101, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
};

static Button gDeleteButtons[] = {
    { { SCREEN_WIDTH / 2 - 150, SCREEN_HEIGHT / 2 + 30 }, { 140.0f , 45.0f }, "YES", 102, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
    { { SCREEN_WIDTH / 2 + 10 , SCREEN_HEIGHT / 2 + 30 }, { 140.0f , 45.0f }, "NO", 103, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f },
};

static Button gOKButton = { { SCREEN_WIDTH / 2 - 70, SCREEN_HEIGHT / 2 + 30 }, { 140.0f , 45.0f }, "OK", 104, BUTTON_VISUAL_TEXT, BUTTON_ICON_NONE, 20.0f, 1.0f };

// CẤU HÌNH GIAO DIỆN (Đồng bộ với ui_save)
static constexpr float LOAD_PANEL_W = 800.0f;
static constexpr float LOAD_PANEL_H = 600.0f;

static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color)
{
    if (text == nullptr || text[0] == '\0') return;
    Vector2 sz = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(font, text,
        Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
        fontSize, 1.0f, color);
}

// Trong ui_load.cpp hoặc file chứa định nghĩa button

void InitLoadUI()
{
    gScrollY = 0.0f;
    gSelectedKey = "";
    LoadGamesFromFile(gameSaves);
}

void ShutdownLoadUI()
{
    // Giải phóng tài nguyên nếu cần
}

static bool ActionRenameSave() {
    string oldKey = gSelectedKey;
    string newKey = string(gRenameBuffer);

    if (newKey.empty()) {
        gRenameStatusMsg = "";
        return false;
    }
    if (gameSaves.count(newKey) && newKey != oldKey)
    {
        gRenameStatusMsg = "";
        return false;
    }

    // Thực hiện đổi tên trong Map
	RenameLoadedGame(oldKey, newKey); // Cập nhật tên trong currentGame nếu đang load game đó
    gSelectedKey = newKey;      // Cập nhật lựa chọn hiện tại sang tên mới
    gShowRenameOverlay = false; // Đóng bảng
	return true;
}

static void ActionDeleteSave() {
    if (!gSelectedKey.empty()) {
        DeleteGameSave(gSelectedKey); // Xóa bản lưu khỏi file và map
        SaveGamesToFile(gameSaves); // Ghi lại file để đảm bảo bản lưu đã bị xóa hoàn toàn
        gSelectedKey = ""; // Xóa xong thì không chọn gì nữa
        gShowDeleteConfirm = false;
    }
}

static void DrawNotificationMessageUI(Font fontTitle, Font fontSmall, const char* messageLine1, const char* messageLine2, Color color) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 180 });

    Rectangle panel = { SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 120, 600, 240 };
    DrawPanelFrame(panel);
    DrawCenteredText(fontTitle, messageLine1, SCREEN_HEIGHT * 0.5f - 80.0f, 30.0f, color);
    DrawCenteredText(fontSmall, messageLine2, SCREEN_HEIGHT * 0.5f - 45.0f, 30.0f, color);

    Rectangle btnRect = GetButtonRect(gOKButton);
    bool hov = CheckCollisionPointRec(GetMousePosition(), btnRect);
    bool prs = hov && IsMouseButtonDown(MOUSE_LEFT_BUTTON);

    DrawUIButton(104, gOKButton, fontSmall, hov, prs);
}

static void DrawLoadOverlays(Font fontTitle, Font fontSmall, const MouseState& mouse) {
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 180 });

    if (gShowRenameOverlay) {
        Rectangle panel = { SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 120, 600, 240 };
        DrawPanelFrame(panel);
        DrawCenteredText(fontTitle, "RENAME SAVE", panel.y + 20, 30, GOLD);

        if (gRenameStatusMsg[0] != '\0')
            DrawCenteredText(fontSmall, gRenameStatusMsg, panel.y + 60, 20, Color {160, 20, 20, 255});

        Rectangle box = { panel.x + 50, panel.y + 75, 500, 50 };
        DrawRectangleRec(box, Color{ 30, 30, 40, 255 });
        DrawRectangleLinesEx(box, 2.0f, RAYWHITE);

        Vector2 textPos = { box.x + 15, box.y + 12 };
        if (gRenameLetterCount == 0)
        {
            DrawTextEx(fontSmall, gSelectedKey.c_str(), textPos, 22.0f, 1.0f, GRAY);
        }
        else
        {
            DrawTextEx(fontSmall, gRenameBuffer, { box.x + 15, box.y + 12 }, 22.0f, 1.0f, GOLD);
        }

        if (fmodf(gCursorBlinkTimer, 1.0f) < 0.5f /* &&!gShouldExitAfterSave */)
        {
            Vector2 textSize = MeasureTextEx(fontSmall, gRenameBuffer, 22.0f, 1.0f);
            DrawTextEx(fontSmall, "_", Vector2{ textPos.x + textSize.x + 2.0f, textPos.y }, 22.0f, 1.0f, GOLD);
        }
        // Định nghĩa nút theo đúng kiểu của bạn

        for (int j = 0; j < 2; j++) {
            Rectangle r = { gRenameButtons[j].position.x, gRenameButtons[j].position.y, gRenameButtons[j].size.x, gRenameButtons[j].size.y };
            bool hov = CheckCollisionPointRec(mouse.position, r);
            // Dùng ID 100+j để không trùng với ID nút main của LoadUI
            DrawUIButton(100 + j, gRenameButtons[j], fontSmall, hov, hov && mouse.leftDown);
        }
    }

    if (gShowDeleteConfirm) {
        Rectangle panel = { SCREEN_WIDTH / 2 - 300, SCREEN_HEIGHT / 2 - 120, 600, 240 };
        DrawPanelFrame(panel);
        DrawCenteredText(fontSmall, "ARE YOU SURE YOU WANT TO DELETE THIS GAME?", panel.y + 40, 35, DARKPURPLE);
        DrawCenteredText(fontSmall, ("\"" + gSelectedKey + "\"").c_str(), panel.y + 80, 30, DARKPURPLE);

        for (int j = 0; j < 2; j++) {
            Rectangle r = { gDeleteButtons[j].position.x, gDeleteButtons[j].position.y, gDeleteButtons[j].size.x, gDeleteButtons[j].size.y };
            bool hov = CheckCollisionPointRec(mouse.position, r);
            DrawUIButton(102 + j, gDeleteButtons[j], fontSmall, hov, hov && mouse.leftDown);
        }
    }
}

void UpdateLoadUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    // --- TÍNH TOÁN PANEL GIỐNG HÀM DRAW ---
    float panelW = 800.0f;
    float panelH = 600.0f;
    Rectangle panel = { SCREEN_WIDTH * 0.5f - panelW * 0.5f, SCREEN_HEIGHT * 0.5f - panelH * 0.5f, panelW, panelH };
    Rectangle container = { panel.x + 40, panel.y + 100, 480, VIEW_HEIGHT };

    if (gShowNotification) {
        Rectangle btnRect = GetButtonRect(gOKButton);
        bool hov = CheckCollisionPointRec(mouse.position, btnRect);

        // Chỉ đơn thuần là đóng thông báo để quay lại màn hình Load
        if ((hov && mouse.leftReleased)) {
            PlayMenuClick(audio, settings);
            gShowNotification = false;
        }
        return;
    }

    if (gShowRenameOverlay) {
        // Cập nhật bộ đếm thời gian cho con trỏ nhấp nháy
        gCursorBlinkTimer += dt;

        // 1. Lấy ký tự vừa nhấn
        int key = GetCharPressed();
        while (key > 0) {
            // Chỉ nhận các ký tự in ấn được (A-Z, 0-9, space,...) và giới hạn 31 ký tự
            if (IsValidChar((char)key) && (gRenameLetterCount < 31)) {
                gRenameBuffer[gRenameLetterCount] = (char)key;
                gRenameLetterCount++;
                gRenameBuffer[gRenameLetterCount] = '\0'; // Kết thúc chuỗi
            }
            key = GetCharPressed(); // Lấy ký tự tiếp theo trong hàng đợi
        }

        // 2. Xử lý phím xóa (Backspace)
        if (IsKeyPressed(KEY_BACKSPACE) && gRenameLetterCount > 0) {
            gRenameLetterCount--;
            gRenameBuffer[gRenameLetterCount] = '\0';
        }

        // 3. Xử lý nút Confirm và Cancel
        Rectangle confirmRect = { gRenameButtons[0].position.x, gRenameButtons[0].position.y, gRenameButtons[0].size.x, gRenameButtons[0].size.y };
        Rectangle cancelRect = { gRenameButtons[1].position.x, gRenameButtons[1].position.y, gRenameButtons[1].size.x, gRenameButtons[1].size.y };
        bool confirmClicked = (CheckCollisionPointRec(mouse.position, confirmRect) && mouse.leftReleased);
        if (confirmClicked) {
            PlayMenuClick(audio, settings);
            string temp = gSelectedKey;
            if (ActionRenameSave()) {
                gNotificationMessageLine1 = "RENAMED GAME \"" + temp + "\"";
				gNotificationMessageLine2 = "TO GAME \"" + gSelectedKey + "\" SUCCESSFULLY!";
				gRenameBuffer[0] = '\0'; // Reset buffer sau khi đổi tên thành công
				gRenameLetterCount = 0; // Reset đếm ký tự

                // Sau khi đổi tên, cập nhật lại danh sách ngay lập tức
                gameSaves.clear();
                LoadGamesFromFile(gameSaves);
            }
            else {
                gNotificationMessageLine1 = "FAILED TO RENAME GAME!";
				gNotificationMessageLine2 = "NAME IS EITHER EMPTY OR ALREADY EXISTS.";
			}
            gShowNotification = true; // Hiển thị thông báo sau khi đổi tên
        }
        else if (CheckCollisionPointRec(mouse.position, cancelRect) && mouse.leftReleased) {
            PlayMenuClick(audio, settings);
            gShowRenameOverlay = false;
        }
        return;
    }

    if (gShowDeleteConfirm) {
        Rectangle yesRect = { gDeleteButtons[0].position.x, gDeleteButtons[0].position.y, gDeleteButtons[0].size.x, gDeleteButtons[0].size.y };
        Rectangle noRect = { gDeleteButtons[1].position.x, gDeleteButtons[1].position.y, gDeleteButtons[1].size.x, gDeleteButtons[1].size.y };
        bool yesClicked = (CheckCollisionPointRec(mouse.position, yesRect) && mouse.leftReleased);
        bool noClicked = (CheckCollisionPointRec(mouse.position, noRect) && mouse.leftReleased);
        if (yesClicked) {
            PlayMenuClick(audio, settings);
			gNotificationMessageLine1 = "DELETED GAME \"" + gSelectedKey + "\" SUCCESSFULLY!";
            ActionDeleteSave();
            // Sau khi xóa, cập nhật lại danh sách ngay lập tức
            gameSaves.clear();
            LoadGamesFromFile(gameSaves);
            gShowNotification = true; // Hiển thị thông báo sau khi xóa
        }
        else if (noClicked) {
            PlayMenuClick(audio, settings);
            gShowDeleteConfirm = false;
        }
        return;
    }

    // --- 1. XỬ LÝ CUỘN CHUỘT ---
    float wheel = GetMouseWheelMove();
    if (wheel != 0) gScrollY += wheel * 45.0f;

    float totalContentHeight = gameSaves.size() * ITEM_HEIGHT;
    float maxScroll = -(totalContentHeight - container.height + 20);
    if (totalContentHeight > container.height) {
        if (gScrollY < maxScroll) gScrollY = maxScroll;
    }
    else gScrollY = 0;
    if (gScrollY > 0) gScrollY = 0;

    // --- 2. KIỂM TRA CLICK CHỌN ITEM ---
    int i = 0;
    for (auto const& [name, data] : gameSaves) {
        if (name == "" || name[0] == '\0')
        {
            continue;
        }
        Rectangle itemRect = { container.x + 10, container.y + 10 + (i * ITEM_HEIGHT) + gScrollY, container.width - 30, ITEM_HEIGHT - 10 };

        if (CheckCollisionPointRec(mouse.position, container)) {
            if (CheckCollisionPointRec(mouse.position, itemRect) && mouse.leftPressed) {
                PlayMenuClick(audio, settings);
                gSelectedKey = name;
            }
        }
        i++;
    }

    // --- 3. XỬ LÝ NÚT CONFIRM TRÊN MÀN HÌNH ---
    for (int j = 0; j < gLoadButtonCount; j++) {
        bool h, p;
        UpdateUIButton(j, gLoadButtons[j], mouse, dt, audio, settings, h, p);

        if (h && mouse.leftReleased) {
            PlayMenuClick(audio, settings);

            switch (gLoadButtons[j].id) {
            case LOAD_BTN_CONFIRM: // Nút LOAD
                if (!gSelectedKey.empty()) {
                    current() = gameSaves[gSelectedKey]; // Cập nhật dữ liệu game hiện tại với dữ liệu đã chọn
                    currentScreen = SCREEN_PLAY;
                }
                break;

                // Trong switch (gLoadButtons[j].id) của UpdateLoadUI
            case LOAD_BTN_RENAME:
                if (!gSelectedKey.empty()) {
                    gShowRenameOverlay = true;
                    gRenameStatusMsg = "";
                }
                break;

            case LOAD_BTN_DELETE: // Thêm nút Delete
                if (!gSelectedKey.empty()) gShowDeleteConfirm = true;
                break;

            case LOAD_BTN_BACK:
                currentScreen = SCREEN_MAIN_MENU;
                break;
            }
        }
    }
}

void DrawLoadUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings)
{
    DrawBackgroundOnly();
    float panelW = 800.0f;
    float panelH = 600.0f;
    Rectangle panel = { SCREEN_WIDTH * 0.5f - panelW * 0.5f, SCREEN_HEIGHT * 0.5f - panelH * 0.5f, panelW, panelH };

    DrawPanelFrame(panel);

    DrawCenteredText(fontTitle, "LOAD GAME", panel.y + 35.0f, 40.0f, Color{ 255, 235, 225, 255 });

    // Khung danh sách
    Rectangle container = { panel.x + 40, panel.y + 100, 480, VIEW_HEIGHT };
    DrawRectangleRec(container, Color{ 20, 20, 30, 255 });
    DrawRectangleLinesEx(container, 2.0f, RAYWHITE);

    BeginScissorMode((int)container.x, (int)container.y, (int)container.width, (int)container.height);
    int i = 0;
    for (auto& pair : gameSaves) {
        DataGame& d = pair.second;
        Rectangle itemRect = { container.x, container.y + i * ITEM_HEIGHT + gScrollY, container.width, ITEM_HEIGHT };

        // Chỉ vẽ nếu item nằm trong vùng nhìn thấy của container
        if (itemRect.y + ITEM_HEIGHT > container.y && itemRect.y < container.y + container.height) {
            bool hovered = IsMouseOverRect(mouse, itemRect);
            bool selected = (gSelectedKey == pair.first);

            // Vẽ nền và khung cho Item
            DrawRectangleRec(itemRect, selected ? Color{ 45, 50, 65, 255 } : (hovered ? Color{ 35, 40, 50, 255 } : Color{ 25, 30, 40, 255 }));
            DrawRectangleLinesEx(itemRect, 1.0f, selected ? GOLD : DARKGRAY);

            float startX = itemRect.x + 15;
            float currY = itemRect.y + 10;
            float lineSpacing = 25.0f; // Khoảng cách giữa các dòng

            // --- DÒNG 1: TÊN VÁN & THỜI GIAN ---
            DrawTextEx(fontSmall, TextFormat("SAVE: %s", d.nameGame), { startX, currY }, 25, 1, GOLD);
            // Giả sử bạn có biến d.saveTime, nếu chưa có hãy dùng tạm text mặc định
            struct tm timeInfo;
            localtime_s(&timeInfo, &d.saveTime); // Sử dụng localtime_s cho an toàn trên Windows

            // 2. Tạo buffer để chứa chuỗi thời gian đã định dạng
            char timeBuffer[20];
            // Định dạng: %d (ngày), %m (tháng), %Y (năm), %H (giờ), %M (phút)
            strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M", &timeInfo);

            DrawTextEx(fontSmall, timeBuffer, { itemRect.x + itemRect.width - 150, currY + 25.0f }, 23, 1, GRAY);

            // --- DÒNG 2: NGƯỜI CHƠI 1 & NGƯỜI CHƠI 2 (KÈM ĐIỂM) ---
            currY += lineSpacing;
            DrawTextEx(fontSmall, TextFormat("P1: %s (%d)", d.namePlayer1, d.scorePlayer1), { startX, currY }, 23, 1, WHITE);
            DrawTextEx(fontSmall, TextFormat("P2: %s (%d)", d.namePlayer2, d.scorePlayer2), { startX + 200, currY }, 23, 1, WHITE);

            // --- DÒNG 3: CHẾ ĐỘ CHƠI & ĐỘ KHÓ BOT ---
            currY += lineSpacing;
            if (d.gameMode == MODE_PVP) {
                const char* pvpModeStr = (d.pvpMode == CLASSIC) ? "PVP Mode: Classic" : "PVP Mode: Tournament";
                DrawTextEx(fontSmall, pvpModeStr, { startX, currY }, 23, 1, VIOLET);

                currY += lineSpacing;

                if (d.scorePlayer1 >= 5 || d.scorePlayer2 >= 5 || d.result != RESULT_ONGOING) {
                    const char* resStr = "STATUS: GAME DONE";
                    DrawTextEx(fontSmall, resStr, { startX, currY }, 23, 1, LIME);
                }
                else {
                    DrawTextEx(fontSmall, "STATUS: IN PROGRESS...", { startX, currY }, 23, 1, ORANGE);
                }
            }

            else if (d.gameMode == MODE_PVE) {
                const char* pveModeStr = TextFormat("Mode: PVE (vs Bot)  %s", (d.botDifficulty == DIFFICULTY_EASY ? "Easy" : (d.botDifficulty == DIFFICULTY_HARD ? "Hard" : "Medium")));
                DrawTextEx(fontSmall, pveModeStr, { startX, currY }, 23, 1, SKYBLUE);

                currY += lineSpacing;

                if (d.scorePlayer1 >= 5 || d.scorePlayer2 >= 5 || d.result != RESULT_ONGOING) {


                    const char* resStr = (d.scorePlayer1 > d.scorePlayer2 ? "STATUS: WINNING" : (d.scorePlayer2 > d.scorePlayer1 ? "STATUS: LOSING" : "STATUS: DRAW"));
                    DrawTextEx(fontSmall, resStr, { startX, currY }, 23, 1, LIME);
                }
                else {
                    DrawTextEx(fontSmall, "STATUS: IN PROGRESS...", { startX, currY }, 23, 1, ORANGE);
                }
            }
        }
        i++;
    }

    EndScissorMode();

    // Khung Preview & Nút Confirm
    Rectangle previewBox = { container.x + container.width + 20, container.y, 220, VIEW_HEIGHT };

    for (int j = 0; j < gLoadButtonCount; j++) {
        Rectangle btnRect = GetButtonRect(gLoadButtons[j]);

        // Kiểm tra hover và pressed để tạo hiệu ứng thị giác (nút vẫn lún xuống khi bấm)
        bool hov = IsMouseOverRect(mouse, btnRect);
        bool prs = hov && mouse.leftDown;

        // Vẽ nút bình thường (không có lớp phủ xám)
        DrawUIButton(j, gLoadButtons[j], fontSmall, hov, prs);
    }

    // (Tùy chọn) Hiện dòng chữ nhắc nhở nhẹ nhàng nếu chưa chọn game
    if (gSelectedKey.empty()) {
        DrawCenteredText(fontSmall, "(Please select a save to Load/Rename)", panel.y + panel.height - 60.0f, 18, WHITE);
    }

    if (gShowRenameOverlay || gShowDeleteConfirm) {
        DrawLoadOverlays(fontTitle, fontSmall, mouse);
    }

    // Hiển thị thông báo
    if (gShowNotification) {
        DrawNotificationMessageUI(fontTitle, fontSmall, gNotificationMessageLine1.c_str(), gNotificationMessageLine2.c_str(), DARKGREEN);
    }
}