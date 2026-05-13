#include"ui_load.h"
#include< string>
using namespace std;

static float gScrollY = 0.0f;
static string gSelectedKey = "";
static const float ITEM_HEIGHT = 90.0f;
static const float VIEW_HEIGHT = 450.0f;

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
void UpdateLoadButtonPositions(Rectangle panel) {
    float previewStartX = panel.x + panel.width - 280.0f; // Căn lề phải trong panel
    float startY = panel.y + 100.0f;
    float spacingY = 65.0f;
    float btnWidth = 240.0f;
    float btnHeight = 45.0f;

    // 4 nút thông số
    for (int i = 0; i < 4; i++) {
        gLoadButtons[i].position = { previewStartX, startY + i * spacingY };
        gLoadButtons[i].size = { btnWidth, btnHeight };
    }

    // Nút CONFIRM lớn (màu đỏ trong hình)
    gLoadButtons[LOAD_BTN_CONFIRM].position = { previewStartX, startY + 4 * spacingY + 120.0f };
    gLoadButtons[LOAD_BTN_CONFIRM].size = { btnWidth, 50.0f };

    // Nút BACK (ở dưới cùng màn hình như ui_save)
    gLoadButtons[LOAD_BTN_BACK].position = { SCREEN_WIDTH * 0.5f - 160.0f, SCREEN_HEIGHT - 100.0f };
    gLoadButtons[LOAD_BTN_BACK].size = { 320.0f, 60.0f };
}

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
    for (int j = LOAD_BTN_CONFIRM; j <= LOAD_BTN_BACK; j++) {
        bool h, p; // Biến nhận trạng thái hover và pressed từ hệ thống UI chung

        // Gọi hàm hệ thống bạn đã cung cấp để cập nhật hiệu ứng (phình to, âm thanh hover)
        UpdateUIButton(j, gLoadButtons[j], mouse, dt, audio, settings, h, p);

        // Xử lý logic khi nút thực sự được Click (Chuột nhả ra trên nút)
        if (h && mouse.leftReleased) {
            switch (gLoadButtons[j].id) {
            case LOAD_BTN_CONFIRM:
                if (!gSelectedKey.empty()) {
                    PlayMenuClick(audio, settings);
                    GetCurrentGameData() = gameSaves[gSelectedKey];
                    currentScreen = SCREEN_PLAY;
                }
                break;

            case LOAD_BTN_BACK:
                PlayMenuClick(audio, settings);
                currentScreen = SCREEN_MAIN_MENU;
                break;

                // Các nút thông số (LOAD_BTN_GN, GP1, GP2, GMODE) không có logic click
                // Không thêm hiệu ứng nhấn tại đây.
            default: break;
            }
        }
    }
    // --- 4. XỬ LÝ PHÍM TẮT ---
    if (!gSelectedKey.empty() && (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))) {
        GetCurrentGameData() = gameSaves[gSelectedKey];
        currentScreen = SCREEN_PLAY;
    }
    if (IsKeyPressed(KEY_ESCAPE)) currentScreen = SCREEN_MAIN_MENU;
}

void DrawLoadUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings)
{
    DrawBackgroundOnly();
    float panelW = 800.0f;
    float panelH = 600.0f;
    Rectangle panel = { SCREEN_WIDTH * 0.5f - panelW * 0.5f, SCREEN_HEIGHT * 0.5f - panelH * 0.5f, panelW, panelH };

    DrawPanelFrame(panel);

    UpdateLoadButtonPositions(panel);
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
            DrawTextEx(fontSmall, TextFormat("SAVE: %s", d.nameGame), { startX, currY }, 22, 1, GOLD);
            // Giả sử bạn có biến d.saveTime, nếu chưa có hãy dùng tạm text mặc định
            struct tm timeInfo;
            localtime_s(&timeInfo, &d.saveTime); // Sử dụng localtime_s cho an toàn trên Windows

            // 2. Tạo buffer để chứa chuỗi thời gian đã định dạng
            char timeBuffer[20];
            // Định dạng: %d (ngày), %m (tháng), %Y (năm), %H (giờ), %M (phút)
            strftime(timeBuffer, sizeof(timeBuffer), "%d/%m/%Y %H:%M", &timeInfo);

            DrawTextEx(fontSmall, timeBuffer, { itemRect.x + itemRect.width - 150, currY + 25.0f }, 18, 1, GRAY);

            // --- DÒNG 2: NGƯỜI CHƠI 1 & NGƯỜI CHƠI 2 (KÈM ĐIỂM) ---
            currY += lineSpacing;
            DrawTextEx(fontSmall, TextFormat("P1: %s (%d)", d.namePlayer1, d.scorePlayer1), { startX, currY }, 18, 1, WHITE);
            DrawTextEx(fontSmall, TextFormat("P2: %s (%d)", d.namePlayer2, d.scorePlayer2), { startX + 200, currY }, 18, 1, WHITE);

            // --- DÒNG 3: CHẾ ĐỘ CHƠI & ĐỘ KHÓ BOT ---
            currY += lineSpacing;
            const char* modeStr = (d.gameMode == MODE_PVP) ? "Mode: PVP (2 Player)" : "Mode: PVE (vs Bot)";
            DrawTextEx(fontSmall, modeStr, { startX, currY }, 18, 1, LIGHTGRAY);

            if (d.gameMode == MODE_PVE) {
                const char* diffStr = TextFormat("Bot: %s", (d.botDifficulty == DIFFICULTY_EASY ? "Easy" : (d.botDifficulty == DIFFICULTY_HARD ? "Hard" : "Medium")));
                DrawTextEx(fontSmall, diffStr, { startX + 250, currY }, 18, 1, SKYBLUE);
            }

            // --- DÒNG 4: KẾT QUẢ (NẾU CÓ) ---
            currY += lineSpacing;
            if (d.scorePlayer1 >= 5 || d.scorePlayer2 >= 5 || d.result != RESULT_ONGOING) {
                const char* resStr = (d.scorePlayer1 >= 5) ? "WINNER: PLAYER 1" : (d.scorePlayer2 >= 5 ? "WINNER: PLAYER 2" : "DRAW");
                DrawTextEx(fontSmall, resStr, { startX, currY }, 18, 1, LIME);
            }
            else {
                DrawTextEx(fontSmall, "Status: In Progress...", { startX, currY }, 18, 1, ORANGE);
            }

            // Click để chọn
            if (hovered && mouse.leftPressed) gSelectedKey = pair.first;
        }
        i++;
    }
    
    EndScissorMode();

    // Khung Preview & Nút Confirm
    Rectangle previewBox = { container.x + container.width + 20, container.y, 220, VIEW_HEIGHT };

    if (!gSelectedKey.empty()) {
        DataGame& d = gameSaves[gSelectedKey];
        DrawTextEx(fontSmall, "PREVIEW", { previewBox.x + 15, previewBox.y + 15 }, 22, 1, GOLD);

        gLoadButtons[LOAD_BTN_GN].title = TextFormat("Game: %s", d.nameGame);
        gLoadButtons[LOAD_BTN_GP1].title = TextFormat("P1: %s", d.namePlayer1);
        gLoadButtons[LOAD_BTN_GP2].title = TextFormat("P2: %s", d.namePlayer2);
        gLoadButtons[LOAD_BTN_GMODE].title = TextFormat("Mode: %s", (d.gameMode == MODE_PVP ? "PVP" : "PVE"));

        for (int j = 0; j < 4; j++) {
            // Ép hovered = false, pressed = false
            DrawUIButton(j, gLoadButtons[j], fontSmall, false, false);
        }

        // 2. Vẽ nút CONFIRM: Giữ nguyên logic tương tác
        bool hovConf = IsMouseOverRect(mouse, GetButtonRect(gLoadButtons[LOAD_BTN_CONFIRM]));
        bool prsConf = hovConf && mouse.leftDown;
        DrawUIButton(LOAD_BTN_CONFIRM, gLoadButtons[LOAD_BTN_CONFIRM], fontSmall, hovConf, prsConf);

        // Luôn vẽ nút BACK
        bool hovBack = IsMouseOverRect(mouse, GetButtonRect(gLoadButtons[LOAD_BTN_BACK]));
        bool prsBack = hovBack && mouse.leftDown;
        DrawUIButton(LOAD_BTN_BACK, gLoadButtons[LOAD_BTN_BACK], fontSmall, hovBack, prsBack);
    }
}