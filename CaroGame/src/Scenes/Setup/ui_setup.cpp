#include "Scenes/Setup/ui_setup.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/config.h"

// Khai báo biến Texture toàn cục cho màn hình Setup
static Texture2D texMainPanel;
static Texture2D texBossFrame;
static Texture2D texSizeFrame;

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
    InitUIFrameSystem();
}

void ShutdownSetupUI()
{
    ShutdownUIFrameSystem();
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

    // ==========================================
    // 1. GỌI THƯ VIỆN VIEW ĐỂ VẼ 3 CÁI KHUNG
    // (Tọa độ tui ước lượng, ông tự canh lại cho khít nha)
    // ==========================================
    // Khung bự chảng ở dưới
    DrawPanelFrame({ 200.0f, 150.0f, 1200.0f, 600.0f });

    // Khung vàng bự chứa Boss
    DrawCardFrame({ 420.0f, 250.0f, 700.0f, 350.0f });

    // Khung nhỏ chứa chữ X 
    DrawSmallFrame({ 620.0f, 450.0f, 200.0f, 400.0f });

    // ==========================================
    // 2. XỬ LÝ LOGIC CHỮ (Controller)
    // ==========================================
    const char* titleText = "";
    const char* descText = "";
    const char* detailText = "";

    if (settings.gameMode == 1) {
        if (settings.botDifficulty == 1) { titleText = "BOSS 1"; descText = "INTERN"; detailText = "De nhu an keo"; }
        else if (settings.botDifficulty == 2) { titleText = "BOSS 2"; descText = "SENIOR"; detailText = "Khong de bi lua dau"; }
        else if (settings.botDifficulty == 3) { titleText = "BOSS 3"; descText = "TECH LEAD"; detailText = "Doc co cau bai"; }
    }
    else {
        if (settings.pvpMode == 1) { titleText = "CLASSIC"; descText = ""; detailText = "-Du 5 quan lien tiep la thang\n-Khong ap dung luat chan 2 dau"; }
        else if (settings.pvpMode == 2) { titleText = "TOURNAMENT"; descText = ""; detailText = "-5 quan lien tiep bi chan 2 dau khong tinh la thang\n-Het thoi gian doi luot cho doi thu"; }
    }

    // ==========================================
    // 3. IN CHỮ ĐÈ LÊN KHUNG
    // ==========================================
    DrawTextEx(fontTitle, titleText, Vector2{ 650.0f, 270.0f }, 36.0f, 2.0f, Color{ 128, 0, 32, 255 });
    DrawTextEx(fontSmall, descText, Vector2{ 650.0f, 320.0f }, 24.0f, 2.0f, Color{ 128, 0, 32, 255 });
    // Tọa độ in chữ = Tọa độ Khung Vàng + Padding (canh lề)
    Vector2 textPos = { 540.0f + 40.0f, 400.0f + 40.0f };

    // Gọi hàm in chữ với font nhỏ (fontSmall)
    DrawTextEx(
        fontSmall,
        detailText,
        textPos,
        22.0f,    // Kích thước chữ (Chỉnh nhỏ lại nếu chữ bị tràn khung)
        2.0f,     // Khoảng cách giữa các chữ
        Color{ 130, 40, 60, 255 } // Màu đỏ đô tiệp với viền nút
    );

    //const char* sizeStr = TextFormat("%dx%d", settings.boardSize, settings.boardSize);
    //DrawTextEx(fontSmall, sizeStr, Vector2{ 440.0f, 495.0f }, 20.0f, 2.0f, Color{ 128, 0, 32, 255 });

    // ==========================================
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