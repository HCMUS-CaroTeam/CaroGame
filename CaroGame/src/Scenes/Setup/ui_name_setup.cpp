#include "Scenes/Setup/ui_name_setup.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/game_data.h"
#include "Model/config.h"
#include <cstring>
#include <cmath>

// ============================================================
//  AVATAR
// ============================================================

static constexpr int AVATAR_COUNT = 4;
static Texture2D gAvatarTex[AVATAR_COUNT] = {};
static bool      gAvatarLoaded[AVATAR_COUNT] = {};

static Texture2D gBotAvatarTex = {};
static bool      gBotAvatarLoaded = false;

static Texture2D gLabelTex = {};
static bool      gLabelTexLoaded = false;
static Texture2D gArrowLeftIdle = {};   
static Texture2D gArrowLeftPress = {};   
static Texture2D gArrowRightIdle = {};   
static Texture2D gArrowRightPress = {};   
static bool      gArrowTexLoaded = false;

// Mỗi người chọn avatar index (0-3)
static int gAvatarP1 = 0;
static int gAvatarP2 = 1;

// ============================================================
//  INPUT BUFFER
// ============================================================

// Buffer nhập tên — hoàn toàn độc lập với gInputBuffer của ui_save
static char gNameBuf1[50] = "";
static char gNameBuf2[50] = "";
static int  gNameLen1 = 0;
static int  gNameLen2 = 0;
static int  gActiveField = 0;   // 0 = P1, 1 = P2

static float gCursorTimer = 0.0f;

// ============================================================
//  LAYOUT CONSTANTS  (chỉnh tại đây để thay đổi giao diện)
// ============================================================

// Panel PVP
static constexpr float PANEL_X = SCREEN_WIDTH * 0.5f - 500.0f;
static constexpr float PANEL_Y = 120.0f;
static constexpr float PANEL_W = 1000.0f;
static constexpr float PANEL_H = 600.0f;


// Avatar display
static constexpr float AVA_SIZE = 150.0f;
static constexpr float AVA_P1_X = PANEL_X + PANEL_W * 0.25f - AVA_SIZE * 0.5f;   // ← P1 cột trái
static constexpr float AVA_P2_X = PANEL_X + PANEL_W * 0.25f - AVA_SIZE * 0.5f;  // ← P2 cùng cột (dọc)
static constexpr float AVA_Y = PANEL_Y + 130.0f;   // ← Y hàng P1
static constexpr float ROW2_OFFSET = 200.0f;              // ← khoảng cách P2 so với P1

// Avatar arrow button hit area (nhỏ, đặt hai bên ô avatar)
static constexpr float ARR_W = 50.0f;
static constexpr float ARR_H = 50.0f;

// Input box
static constexpr float BOX_W = 340.0f;
static constexpr float BOX_H = 50.0f;
static constexpr float BOX_P1_X = PANEL_X + PANEL_W * 0.7f - BOX_W * 0.5f;
static constexpr float BOX_P2_X = PANEL_X + PANEL_W * 0.7f - BOX_W * 0.5f;
static constexpr float BOX_Y = PANEL_Y + 180.0f;

// PANEL PVE
static constexpr float PANEL_PVE_X = SCREEN_WIDTH * 0.5f - 500.0f;
static constexpr float PANEL_PVE_W = 1000.0f;
static constexpr float PANEL_PVE_H = 450.0f;
static constexpr float PANEL_PVE_Y = (SCREEN_HEIGHT - PANEL_PVE_H) * 0.5f;

// Avatar and input box for PvE mode 
static constexpr float AVA_PVE_X = PANEL_PVE_X + PANEL_PVE_W * 0.25f - AVA_SIZE * 0.5f;
static constexpr float AVA_PVE_Y = PANEL_PVE_Y + PANEL_PVE_H * 0.5f - AVA_SIZE * 0.5f;
static constexpr float BOX_PVE_X = PANEL_PVE_X + PANEL_PVE_W * 0.7f - BOX_W * 0.5f;
static constexpr float BOX_PVE_Y = PANEL_PVE_Y + PANEL_PVE_H * 0.5f - BOX_H * 0.5f; 

// Button size
static constexpr float BTN_W = 150.0f;
static constexpr float BTN_H = 54.0f;
static constexpr float BTN_X = (SCREEN_WIDTH - BTN_W) * 0.5f;

// Button position for PvP
static constexpr float BTN_PLAY_PVP_Y = PANEL_Y + PANEL_H * 0.85f;
static constexpr float BTN_BACK_PVP_Y = PANEL_Y + PANEL_H + 30.0f;

// Button position for PvE
static constexpr float BTN_PLAY_PVE_Y = PANEL_PVE_Y + PANEL_PVE_H * 0.8f;
static constexpr float BTN_BACK_PVE_Y = PANEL_PVE_Y + PANEL_PVE_H + 30.0f;


// ============================================================
//  HELPER
// ============================================================

static void DrawCenteredText(Font font, const char* text, float y,
    float fontSize, Color color)
{
    if (!text || text[0] == '\0') return;
    Vector2 sz = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(font, text,
        Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
        fontSize, 1.0f, color);
}

static void DrawLabelFrame(Font font, const char* text,
    float cx, float y, float w, float h, Color textColor)
{
    Rectangle dest = { cx - w * 0.5f, y, w, h };

    if (gLabelTexLoaded) {
        DrawTexturePro(gLabelTex,
            Rectangle{ 0, 0,
                (float)gLabelTex.width,
                (float)gLabelTex.height },
            dest, Vector2{ 0, 0 }, 0.0f, WHITE);
    }
    else {
        // Fallback nếu không load được texture
        DrawRectangleRounded(dest, 0.3f, 8, Color{ 60, 40, 60, 200 });
        DrawRectangleRoundedLinesEx(dest, 0.3f, 8, 1.5f, Color{ 200, 150, 180, 180 });
    }

    Vector2 sz = MeasureTextEx(font, text, 22.0f, 1.0f);
    Vector2 pos = {
        dest.x + dest.width * 0.5f - sz.x * 0.5f,
        dest.y + dest.height * 0.5f - sz.y * 0.5f
    };
    DrawTextEx(font, text, pos, 22.0f, 1.0f, textColor);
}

static void DrawInputBox(Font font, const char* buf, int len,
    float x, float y, bool active, float cursorTimer)
{
    Rectangle box = { x, y, BOX_W, BOX_H };
    Color borderCol = active ? Color{ 255, 203, 0, 255 }   // GOLD khi active
    : Color{ 180, 180, 180, 200 };
    DrawSmallFrame(box);

    if (active)
        DrawRectangleLinesEx(box, 2.0f, Color{ 255, 203, 0, 255 });

    Vector2 textPos = { x + 12.0f, y + 11.0f };
    if (len == 0)
    {
        DrawTextEx(font, "Enter name...", textPos, 22.0f, 1.0f,
            Color{ 130, 130, 130, 200 });
    }
    else
    {
        DrawTextEx(font, buf, textPos, 22.0f, 1.0f, BLACK);
    }

    // Con trỏ nhấp nháy
    if (active && fmodf(cursorTimer, 1.0f) < 0.5f)
    {
        Vector2 sz = MeasureTextEx(font, buf, 22.0f, 1.0f);
        DrawTextEx(font, "_",
            Vector2{ textPos.x + sz.x + 2.0f, textPos.y },
            22.0f, 1.0f, Color{ 255, 203, 0, 255 });
    }
}

// Vẽ ô avatar + nút mũi tên trái/phải → trả về true nếu người chơi vừa
// click vào mũi tên (để phát âm thanh ở ngoài nếu cần).
static void DrawAvatarSelector(Font fontSmall, float ax, float ay,
    int avatarIdx, bool isP2,
    const MouseState& mouse,
    bool& clickedLeft, bool& clickedRight)
{
    clickedLeft = clickedRight = false;

    // Khung avatar — đóng khung như màn play
    Rectangle avatarRect = { ax, ay, AVA_SIZE, AVA_SIZE };
    DrawCardFrame(avatarRect);
    DrawRectangleRounded(
        Rectangle{ ax + 6.0f, ay + 6.0f, AVA_SIZE - 12.0f, AVA_SIZE - 12.0f },
        0.04f, 5, Color{ 206, 218, 170, 210 });

    constexpr float AVA_PAD = 10.0f;
    Rectangle innerRect = { ax + AVA_PAD, ay + AVA_PAD, AVA_SIZE - AVA_PAD * 2.0f, AVA_SIZE - AVA_PAD * 2.0f };

    if (gAvatarLoaded[avatarIdx])
    {
        DrawTexturePro(gAvatarTex[avatarIdx],
            Rectangle{ 0, 0, (float)gAvatarTex[avatarIdx].width,
                             (float)gAvatarTex[avatarIdx].height },
            innerRect, Vector2{ 0,0 }, 0.0f, WHITE);
    }
    else
    {
        // Fallback: vẽ chữ số
        char buf[4]; snprintf(buf, sizeof(buf), "%d", avatarIdx + 1);
        Vector2 sz = MeasureTextEx(fontSmall, buf, 40.0f, 1.0f);
        DrawTextEx(fontSmall, buf,
            Vector2{ ax + AVA_SIZE * 0.5f - sz.x * 0.5f,
                    ay + AVA_SIZE * 0.5f - sz.y * 0.5f },
            40.0f, 1.0f, RAYWHITE);
    }

    // Mũi tên trái
    Rectangle leftBtn = { ax - ARR_W - 8.0f,
                            ay + AVA_SIZE * 0.5f - ARR_H * 0.5f,
                            ARR_W, ARR_H };
    // Mũi tên phải
    Rectangle rightBtn = { ax + AVA_SIZE + 8.0f,
                            ay + AVA_SIZE * 0.5f - ARR_H * 0.5f,
                            ARR_W, ARR_H };

    bool hovL = CheckCollisionPointRec(mouse.position, leftBtn);
    bool hovR = CheckCollisionPointRec(mouse.position, rightBtn);

    // Helper lambda-style inline để chọn đúng texture
    bool pressedL = hovL && mouse.leftDown;
    bool pressedR = hovR && mouse.leftDown;

    if (gArrowTexLoaded) {
        // Nút trái
        const Texture2D& texL = pressedL ? gArrowLeftPress : gArrowLeftIdle;
        DrawTexturePro(texL,
            Rectangle{ 0, 0, (float)texL.width, (float)texL.height },
            leftBtn, Vector2{ 0, 0 }, 0.0f, WHITE);

        // Nút phải
        const Texture2D& texR = pressedR ? gArrowRightPress : gArrowRightIdle;
        DrawTexturePro(texR,
            Rectangle{ 0, 0, (float)texR.width, (float)texR.height },
            rightBtn, Vector2{ 0, 0 }, 0.0f, WHITE);
    }
    else {
        // Fallback nếu không load được texture
        DrawRectangleRec(leftBtn, hovL ? Color{ 80,80,120,220 } : Color{ 40,40,60,180 });
        DrawRectangleRec(rightBtn, hovR ? Color{ 80,80,120,220 } : Color{ 40,40,60,180 });
        DrawRectangleLinesEx(leftBtn, 1.5f, Color{ 200,200,255,180 });
        DrawRectangleLinesEx(rightBtn, 1.5f, Color{ 200,200,255,180 });
        Vector2 szL = MeasureTextEx(fontSmall, "<", 22.0f, 1.0f);
        Vector2 szR = MeasureTextEx(fontSmall, ">", 22.0f, 1.0f);
        DrawTextEx(fontSmall, "<",
            Vector2{ leftBtn.x + leftBtn.width * 0.5f - szL.x * 0.5f,
                     leftBtn.y + leftBtn.height * 0.5f - szL.y * 0.5f },
            22.0f, 1.0f, RAYWHITE);
        DrawTextEx(fontSmall, ">",
            Vector2{ rightBtn.x + rightBtn.width * 0.5f - szR.x * 0.5f,
                     rightBtn.y + rightBtn.height * 0.5f - szR.y * 0.5f },
            22.0f, 1.0f, RAYWHITE);
    }

    if (hovL && mouse.leftPressed)  clickedLeft = true;
    if (hovR && mouse.leftPressed)  clickedRight = true;
}

// ============================================================
//  PUBLIC API
// ============================================================
void InitNameInputUI()
{
    // Load avatar textures
    const char* avatarPaths[AVATAR_COUNT] = {
        "assets/avatars/character-1.png",
        "assets/avatars/character-2.png",
        "assets/avatars/character-3.png",
        "assets/avatars/character-4.png",
    };
    for (int i = 0; i < AVATAR_COUNT; i++)
    {
        if (FileExists(avatarPaths[i]))
        {
            gAvatarTex[i] = LoadTexture(avatarPaths[i]);
            gAvatarLoaded[i] = (gAvatarTex[i].id != 0);
        }
        else
        {
            gAvatarLoaded[i] = false;
        }
    }

    // Boss avatar 
    if (FileExists("assets/avatars/boss.png"))
    {
        gBotAvatarTex = LoadTexture("assets/avatars/boss.png");
        gBotAvatarLoaded = (gBotAvatarTex.id != 0);
    }
    else
    {
        gBotAvatarLoaded = false;
    }

    if (FileExists("assets/buttons/pixil-layer-12.png") &&
        FileExists("assets/buttons/pixil-layer-11.png") &&
        FileExists("assets/buttons/pixil-layer-13.png") &&
        FileExists("assets/buttons/pixil-layer-10.png"))
    {
        gArrowLeftIdle = LoadTexture("assets/buttons/pixil-layer-12.png");
        gArrowLeftPress = LoadTexture("assets/buttons/pixil-layer-11.png");
        gArrowRightIdle = LoadTexture("assets/buttons/pixil-layer-13.png");
        gArrowRightPress = LoadTexture("assets/buttons/pixil-layer-10.png");
        gArrowTexLoaded = true;
    }

    // Reset input
    gNameBuf1[0] = '\0';  gNameLen1 = 0;
    gNameBuf2[0] = '\0';  gNameLen2 = 0;
    gActiveField = 0;
    gCursorTimer = 0.0f;
    gAvatarP1 = 0;
    gAvatarP2 = 1;
}

void ShutdownNameInputUI()
{
    for (int i = 0; i < AVATAR_COUNT; i++)
    {
        if (gAvatarLoaded[i])
        {
            UnloadTexture(gAvatarTex[i]);
            gAvatarTex[i] = Texture2D{};
            gAvatarLoaded[i] = false;
        }
    }
    if (gBotAvatarLoaded) {
        UnloadTexture(gBotAvatarTex);
        gBotAvatarTex = Texture2D{};
        gBotAvatarLoaded = false;
    }
    if (gLabelTexLoaded) {
        UnloadTexture(gLabelTex);
        gLabelTex = Texture2D{};
        gLabelTexLoaded = false;
    }
    if (gArrowTexLoaded) {
        UnloadTexture(gArrowLeftIdle);
        UnloadTexture(gArrowLeftPress);
        UnloadTexture(gArrowRightIdle);
        UnloadTexture(gArrowRightPress);
        gArrowTexLoaded = false;
    }
}

void UpdateNameInputUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    gCursorTimer += dt;

    bool isPvE = (current().gameMode == MODE_PVE);

    // --- Click vào ô nhập để chọn field ---
    Rectangle boxP1, boxP2;
    float avP1x, avP2x, avY1, avY2;

    if (isPvE) {
        boxP1 = { BOX_PVE_X, BOX_PVE_Y, BOX_W, BOX_H };
        avP1x = AVA_PVE_X;
        avY1 = AVA_PVE_Y;
    }
    else {
        boxP1 = { BOX_P1_X, BOX_Y, BOX_W, BOX_H };
        boxP2 = { BOX_P2_X, BOX_Y + ROW2_OFFSET, BOX_W, BOX_H };
        avP1x = AVA_P1_X;
        avP2x = AVA_P2_X;
        avY1 = AVA_Y;
        avY2 = AVA_Y + ROW2_OFFSET;
    }

    if (mouse.leftPressed)
    {
        if (CheckCollisionPointRec(mouse.position, boxP1))
            gActiveField = 0;
        else if (!isPvE && CheckCollisionPointRec(mouse.position, boxP2))
            gActiveField = 1;
    }

    // Nếu PvE thì luôn ghi vào P1
    char* buf = (gActiveField == 0 || isPvE) ? gNameBuf1 : gNameBuf2;
    int& len = (gActiveField == 0 || isPvE) ? gNameLen1 : gNameLen2;

    int key = GetCharPressed();
    while (key > 0)
    {
        if (key >= 32 && key <= 125 && len < 49)
        {
            buf[len++] = (char)key;
            buf[len] = '\0';
        }
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE) && len > 0)
    {
        buf[--len] = '\0';
    }
    // Tab để chuyển field (chỉ PvP)
    if (!isPvE && IsKeyPressed(KEY_TAB))
        gActiveField ^= 1;

    // --- Avatar selector ---
    Rectangle leftP1, rightP1, leftP2, rightP2;
    if (isPvE) {
        leftP1 = { avP1x - ARR_W - 8.0f, avY1 + AVA_SIZE * 0.5f - ARR_H * 0.5f, ARR_W, ARR_H };
        rightP1 = { avP1x + AVA_SIZE + 8.0f, avY1 + AVA_SIZE * 0.5f - ARR_H * 0.5f, ARR_W, ARR_H };
    }
    else {
        leftP1 = { avP1x - ARR_W - 8.0f, avY1 + AVA_SIZE * 0.5f - ARR_H * 0.5f, ARR_W, ARR_H };
        rightP1 = { avP1x + AVA_SIZE + 8.0f, avY1 + AVA_SIZE * 0.5f - ARR_H * 0.5f, ARR_W, ARR_H };
        leftP2 = { avP2x - ARR_W - 8.0f, avY2 + AVA_SIZE * 0.5f - ARR_H * 0.5f, ARR_W, ARR_H };
        rightP2 = { avP2x + AVA_SIZE + 8.0f, avY2 + AVA_SIZE * 0.5f - ARR_H * 0.5f, ARR_W, ARR_H };
    }

    if (mouse.leftPressed)
    {
        if (CheckCollisionPointRec(mouse.position, leftP1))
            gAvatarP1 = (gAvatarP1 + AVATAR_COUNT - 1) % AVATAR_COUNT;
        if (CheckCollisionPointRec(mouse.position, rightP1))
            gAvatarP1 = (gAvatarP1 + 1) % AVATAR_COUNT;
        if (!isPvE)
        {
            if (CheckCollisionPointRec(mouse.position, leftP2))
                gAvatarP2 = (gAvatarP2 + AVATAR_COUNT - 1) % AVATAR_COUNT;
            if (CheckCollisionPointRec(mouse.position, rightP2))
                gAvatarP2 = (gAvatarP2 + 1) % AVATAR_COUNT;
        }
    }

    float btnPlayY, btnBackY, btnX;
    btnX = BTN_X;
    if (!isPvE) {
        btnPlayY = BTN_PLAY_PVP_Y;
        btnBackY = BTN_BACK_PVP_Y;
    }
    else {
        btnPlayY = BTN_PLAY_PVE_Y;
        btnBackY = BTN_BACK_PVE_Y;
    }

    // --- Nút PLAY  ---
    {
        gNameInputButtons[0].position.x = btnX;
        gNameInputButtons[0].position.y = btnPlayY;
        gNameInputButtons[0].size.x = BTN_W;
        gNameInputButtons[0].size.y = BTN_H;

        bool hovered = false, pressed = false;
        UpdateUIButton(NAME_INPUT_ANIM_PLAY, gNameInputButtons[0], mouse, dt, audio, settings, hovered, pressed);
        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);

            const char* p1Name = (gNameLen1 > 0) ? gNameBuf1 : "Player 1";
            strncpy_s(current().namePlayer1, sizeof(current().namePlayer1), p1Name, _TRUNCATE);

            const char* p2Name;
            if (isPvE)
                p2Name = "BOT";
            else
                p2Name = (gNameLen2 > 0) ? gNameBuf2 : "Player 2";
            strncpy_s(current().namePlayer2, sizeof(current().namePlayer2), p2Name, _TRUNCATE);

            currentScreen = SCREEN_PLAY;
        }
    }
    // --- Nút BACK  ---
    {
        gNameInputButtons[1].position.x = btnX;
        gNameInputButtons[1].position.y = btnBackY;
        gNameInputButtons[1].size.x = BTN_W;
        gNameInputButtons[1].size.y = BTN_H;

        bool hovered = false, pressed = false;
        UpdateUIButton(NAME_INPUT_ANIM_BACK, gNameInputButtons[1], mouse, dt, audio, settings, hovered, pressed);
        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);
            currentScreen = SCREEN_SETUP; // Chuyển màn hình về màn Setup
        }
    }

    if (IsKeyPressed(KEY_ESCAPE))
        currentScreen = SCREEN_SETUP;

    // Enter → chuyển field hoặc play
    if (IsKeyPressed(KEY_ENTER))
    {
        if (!isPvE && gActiveField == 0)
            gActiveField = 1;
        else
        {
            // Giống click PLAY
            const char* p1Name = (gNameLen1 > 0) ? gNameBuf1 : "Player 1";
            strncpy_s(current().namePlayer1, sizeof(current().namePlayer1),
                p1Name, _TRUNCATE);
            const char* p2Name = isPvE ? "BOT"
                : (gNameLen2 > 0 ? gNameBuf2 : "Player 2");
            strncpy_s(current().namePlayer2, sizeof(current().namePlayer2),
                p2Name, _TRUNCATE);
            currentScreen = SCREEN_PLAY;
        }
    }
}

void DrawNameInputUI(
    Font fontTitle,
    Font fontSmall,
    const MouseState& mouse,
    const AppSettings& settings
)
{
    DrawBackgroundOnly();

    bool isPvE = (current().gameMode == MODE_PVE);

    if (!isPvE)
    {
        // Panel nền
        DrawPanelFrame({ PANEL_X, PANEL_Y, PANEL_W, PANEL_H });

        // Tiêu đề
        DrawCenteredText(fontTitle, "ENTER PLAYER NAME",
            PANEL_Y + 30.0f, 34.0f, Color{ 255, 235, 225, 255 });

        // ── P1 ──────────────────────────────────────────────────

        // Avatar P1 (xử lý click thực sự đã ở Update, đây chỉ vẽ)
        bool dummyL, dummyR;
        DrawAvatarSelector(fontSmall, AVA_P1_X, AVA_Y, gAvatarP1, false, mouse, dummyL, dummyR);

        // Input box P1
        DrawInputBox(fontSmall, gNameBuf1, gNameLen1,
            BOX_P1_X, BOX_Y, gActiveField == 0, gCursorTimer);

        // ── P2 ──────────────────────────────────────────────────

        DrawTextEx(fontSmall, "Player 1:",
            Vector2{ BOX_P1_X, BOX_Y - 24.0f },
            18.0f, 1.0f, Color{ 120, 220, 255, 200 });
        DrawTextEx(fontSmall, "Player 2:",
            Vector2{ BOX_P2_X, BOX_Y + ROW2_OFFSET - 24.0f },
            18.0f, 1.0f, Color{ 255, 150, 200, 200 });

        DrawAvatarSelector(fontSmall, AVA_P2_X, AVA_Y + ROW2_OFFSET, gAvatarP2, true, mouse, dummyL, dummyR);

        DrawInputBox(fontSmall, gNameBuf2, gNameLen2,
            BOX_P2_X, BOX_Y + ROW2_OFFSET, gActiveField == 1, gCursorTimer);
    }
    else
    {
        // Panel nền
        DrawPanelFrame({ PANEL_PVE_X, PANEL_PVE_Y, PANEL_PVE_W, PANEL_PVE_H });

        // Tiêu đề
        DrawCenteredText(fontTitle, "ENTER PLAYER NAME",
            PANEL_PVE_Y + 30.0f, 34.0f, Color{ 255, 235, 225, 255 });

        DrawTextEx(fontSmall, "Player:",
            Vector2{ BOX_PVE_X, BOX_PVE_Y - 24.0f },
            18.0f, 1.0f, Color{ 120, 220, 255, 200 });
		bool dummyL, dummyR;
		DrawAvatarSelector(fontSmall, AVA_PVE_X, AVA_PVE_Y, gAvatarP1, false, mouse, dummyL, dummyR);
		DrawInputBox(fontSmall, gNameBuf1, gNameLen1,
			BOX_PVE_X, BOX_PVE_Y, true, gCursorTimer);
    }

    {
        float btnPlayY  , btnBackY, btnX;
        btnX = BTN_X;
        if (!isPvE) {
            btnPlayY = BTN_PLAY_PVP_Y;
            btnBackY = BTN_BACK_PVP_Y;
        }
        else {
            btnPlayY = BTN_PLAY_PVE_Y;
            btnBackY = BTN_BACK_PVE_Y;
        }

        gNameInputButtons[0].position.x = btnX;
        gNameInputButtons[0].position.y = btnPlayY;
        gNameInputButtons[0].size.x = BTN_W;
        gNameInputButtons[0].size.y = BTN_H;

        Rectangle hitPlay = { btnX, btnPlayY, BTN_W, BTN_H };
        bool hovPlay = IsMouseOverRect(mouse, hitPlay);
        bool prsPlay = hovPlay && mouse.leftDown;
        DrawUIButton(NAME_INPUT_ANIM_PLAY, gNameInputButtons[0], fontSmall, hovPlay, prsPlay);

        gNameInputButtons[1].position.x = btnX;
        gNameInputButtons[1].position.y = btnBackY;
        gNameInputButtons[1].size.x = BTN_W;
        gNameInputButtons[1].size.y = BTN_H;

        Rectangle hitBack = { btnX, btnBackY, BTN_W, BTN_H };
        bool hovBack = IsMouseOverRect(mouse, hitBack);
        bool prsBack = hovBack && mouse.leftDown;
        DrawUIButton(NAME_INPUT_ANIM_BACK, gNameInputButtons[1], fontSmall, hovBack, prsBack);
    }

}

int GetSelectedAvatarP1()
{
    return gAvatarP1;
}

int GetSelectedAvatarP2()
{
    return gAvatarP2;
}

void ResetNameInputBuffers()
{
    gNameBuf1[0] = '\0'; gNameLen1 = 0;
    gNameBuf2[0] = '\0'; gNameLen2 = 0;
    gActiveField = 0;
    gCursorTimer = 0.0f;
    gAvatarP1 = 0;
    gAvatarP2 = 1;
}
