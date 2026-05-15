#include "ui_story.h"
#include "View/ui_button.h"
#include <cstring>
#include <algorithm>
#include <cmath>

static Texture2D storyBg;
static float timeElapsed = 0.0f;
static bool needReset = true;

static int currentLogIndex = 0;
static const float TYPE_SPEED = 50.0f;

static Button skipBtn = {
    Vector2{ 1350.0f, 820.0f },
    Vector2{ 200.0f, 50.0f },
    "SKIP",
    10,
    BUTTON_VISUAL_TEXT,
    BUTTON_ICON_NONE,
    28.0f,
    2.0f
};

// Cốt truyện chia thành các log
static const char* storyLogs[] = {
    "In a mist-shrouded valley, a dark legend awakens every night."
    "A portal from the Moon opens, unleashing the 'Moon Demon' upon the\n"
    "mortal realm. The creature ravages the village, abducting young"
    "maidens to drain their life force and maintain its immortality.\n"
    "Nobita, an orphan raised by the village chief, is deeply in love"
    "with Shizuka, the chief's beautiful daughter.\n",

    "On a night when the Full Moon shone its brightest, while the village"
    "celebrated a grand festival, the Demon suddenly descended. It seized\n"
    "Shizuka as the villagers watched in terror. Nobita fought with"
    "unmatched fury, but a mere mortal was no match for demonic magic.\n"
    "He watched in agony as she was dragged into the void. Lunging"
    "forward in a desperate attempt to follow, he was violently thrown\n"
    "into the abyss by the portal's shockwave.\n",

    "Surviving the fall, Nobita silently gathered his gear amidst the"
    "ruins of his home. The village chief, with eyes full of sorrow and\n"
    "resolve, imparted his final wisdom: 'The moonlight portal is closed,"
    "but it will soon wane. You have exactly one lunar cycle to train.\n"
    "When the moon becomes a crescent, the demons are at their weakest."
    "That will be your only chance to ascend to the Moon.'",

    "Leaving the flickering lights behind, Nobita embarks on a deadly"
    "training journey to prepare for the Moon's shifting powers.\n"
};

static const int numLogs = sizeof(storyLogs) / sizeof(storyLogs[0]);

void InitStoryScene() {
    // Chỉ nạp nếu texture chưa có dữ liệu (id == 0)
    if (storyBg.id == 0) { 
        storyBg = LoadTexture("Assets/bg/story_bg.png");
    }
    timeElapsed = 0.0f;
    currentLogIndex = 0;
    needReset = true;
}

void UpdateStoryScene(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
) {
    if (needReset) {
        timeElapsed = 0.0f;
        currentLogIndex = 0;
        needReset = false;
    }

    timeElapsed += dt;
    int totalLen = (int)strlen(storyLogs[currentLogIndex]);
    int charsToShow = (int)(timeElapsed * TYPE_SPEED);
    bool isTyping = charsToShow < totalLen;

    // Logic nút SKIP (Chuột Trái) - Bỏ qua tất cả để vào thẳng game
    bool hovered = false;
    bool pressed = false;
    UpdateUIButton(10, skipBtn, mouse, dt, audio, settings, hovered, pressed);

    if (hovered && mouse.leftPressed) {
        currentScreen = SCREEN_SETUP;
        needReset = true;
        return;
    }

    // Logic điều khiển LOG (Chuột Phải)
    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        if (isTyping) {
            // Đang chạy chữ -> Bấm chuột phải để HIỆN HẾT chữ của log này
            timeElapsed = (float)totalLen / TYPE_SPEED + 1.0f;
        }
        else {
            // Đã hiện hết chữ -> Bấm chuột phải để QUA LOG TIẾP THEO
            currentLogIndex++;

            if (currentLogIndex >= numLogs) {
                // Đã hết truyện -> Tự động vào game
                currentScreen = SCREEN_SETUP;
                needReset = true;
            }
            else {
                timeElapsed = 0.0f;
            }
        }
    }
}

void DrawStoryScene(Font fontTitle, Font fontSmall, const MouseState& mouse) {
    (void)fontSmall;

    // 1. Vẽ hình nền gothic tràn màn hình (Sáng sủa, không bị che khuất)
    Rectangle sourceRec = { 0.0f, 0.0f, (float)storyBg.width, (float)storyBg.height };
    Rectangle destRec = { 0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(storyBg, sourceRec, destRec, Vector2{ 0,0 }, 0.0f, WHITE);

    // 2. Vẽ HỘP THOẠI (Dialogue Box) ở phần đáy màn hình
    // Chiều cao hộp thoại là 280 pixel, nằm ở dưới cùng
    float boxY = GetScreenHeight() - 280.0f;
    DrawRectangle(0, (int)boxY, GetScreenWidth(), 280, ColorAlpha(BLACK, 0.65f));

    // Vẽ thêm một đường viền xám mỏng ở mép trên hộp thoại cho đẹp
    DrawRectangle(0, (int)boxY, GetScreenWidth(), 3, ColorAlpha(LIGHTGRAY, 0.5f));

    // Lấy thông tin đoạn log hiện tại
    int totalLen = (int)strlen(storyLogs[currentLogIndex]);
    int charsToShow = (int)(timeElapsed * TYPE_SPEED);
    charsToShow = std::min(charsToShow, totalLen);

    // 3. Vẽ chữ nằm gọn bên trong hộp thoại (Cách lề trái 80px, mép trên hộp thoại 20px)
    DrawTextEx(fontTitle, TextSubtext(storyLogs[currentLogIndex], 0, charsToShow), Vector2{ 50.0f, boxY + 20.0f }, 32.0f, 2.0f, RAYWHITE);

    // 4. Nếu chữ đã hiện ra hết, vẽ thêm chữ nhấp nháy hướng dẫn ở góc dưới bên trái
    if (charsToShow >= totalLen) {
        float alpha = (sinf((float)GetTime() * 5.0f) + 1.0f) / 2.0f;
        // Tọa độ Y mới là boxY + 230 (cách đáy màn hình khoảng 50px)
        DrawTextEx(fontTitle, ">> Right-click to continue <<", Vector2{ 80.0f, boxY + 230.0f }, 22.0f, 2.0f, ColorAlpha(YELLOW, alpha));
    }

    // 5. Vẽ nút SKIP nằm ở góc dưới bên phải của hộp thoại
    const Rectangle hitRect = GetButtonRect(skipBtn);
    bool hovered = IsMouseOverRect(mouse, hitRect);
    bool pressed = hovered && mouse.leftDown;

    DrawUIButton(10, skipBtn, fontTitle, hovered, pressed);
}

void ShutdownStoryScene() {
    UnloadTexture(storyBg);
}