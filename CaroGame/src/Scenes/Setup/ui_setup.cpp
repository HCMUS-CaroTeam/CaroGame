#include "Scenes/Setup/ui_setup.h"
#include "Model/config.h"
#include "Model/logic.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"


// Khai báo biến Texture toàn cục cho màn hình Setup
static Texture2D texMainPanel;
static Texture2D texBossFrame;
static Texture2D texSizeFrame;

static void DrawCenteredText(Font font, const char *text, float y,
                             float fontSize, Color color) {
  Vector2 size = MeasureTextEx(font, text, fontSize, 1.0f);
  DrawTextEx(font, text, Vector2{SCREEN_WIDTH * 0.5f - size.x * 0.5f, y},
             fontSize, 1.0f, color);
}

// Khai báo thêm Texture cho Boss Avatar ở trên cùng (dưới các include)
static Texture2D texBoss1Avatar;
static Texture2D texBoss2Avatar;
static Texture2D texBoss3Avatar;

struct BossData {
    const char* title;
    const char* desc;
    const char* detail;
};

static void DrawBossPreview(Texture2D avatar, Rectangle dialogBounds, BossData data, float scale, Font fontTitle, Font fontSmall) {
    // 1. Vẽ Khung Background
    DrawCardFrame(dialogBounds);

    // Kích thước chuẩn: Nửa trái (Avatar) rộng 220px, Nửa phải (Text) rộng 380px
    float leftWidth = 220.0f;
    float rightWidth = dialogBounds.width - leftWidth;

    // 2. NỬA TRÁI - Avatar (Ép tự động vừa ô)
    Rectangle leftPane = { dialogBounds.x, dialogBounds.y, leftWidth, dialogBounds.height };
    
    if (avatar.id != 0) {
        // Tự động tính tỷ lệ scale sao cho avatar không vượt quá 150 pixel
        float maxAvatarSize = 150.0f;
        float scaleX = maxAvatarSize / static_cast<float>(avatar.width);
        float scaleY = maxAvatarSize / static_cast<float>(avatar.height);
        float finalScale = (scaleX < scaleY) ? scaleX : scaleY; // Lấy scale nhỏ hơn để ko bị méo

        float scaledW = avatar.width * finalScale;
        float scaledH = avatar.height * finalScale;
        
        // Căn giữa hình sau khi đã resize
        float avatarX = leftPane.x + (leftPane.width - scaledW) / 2.0f;
        float avatarY = leftPane.y + (leftPane.height - scaledH) / 2.0f;

        DrawTextureEx(avatar, Vector2{ avatarX, avatarY }, 0.0f, finalScale, WHITE);
    }

    // 3. NỬA PHẢI - Text (Canh trái với padding nhỏ gọn)
    Rectangle rightPane = { dialogBounds.x + leftWidth, dialogBounds.y, rightWidth, dialogBounds.height };
    float textPaddingX = 10.0f;
    float currentY = rightPane.y + 35.0f;

    // Title (Vd: BOSS 2)
    DrawTextEx(fontTitle, data.title, Vector2{ rightPane.x + textPaddingX, currentY }, 32.0f, 2.0f, Color{ 128, 0, 32, 255 });
    currentY += 40.0f;

    // Name (Vd: Half Moon Demon)
    DrawTextEx(fontSmall, data.desc, Vector2{ rightPane.x + textPaddingX, currentY }, 22.0f, 2.0f, Color{ 128, 0, 32, 255 });
    currentY += 45.0f;

    // Description (Vd: - The Demon guard...)
    DrawTextEx(fontSmall, data.detail, Vector2{ rightPane.x + textPaddingX, currentY }, 18.0f, 2.0f, Color{ 130, 40, 60, 255 });
}

void InitSetupUI() { 
    InitUIFrameSystem(); 
    // Load ảnh Boss của bạn ở đây (đổi đường dẫn cho phù hợp)
    texBoss1Avatar = LoadTexture("Assets/avatar/BossPhases(Moon)/pixil-layer-trang-khuyet.png"); 
    texBoss2Avatar = LoadTexture("Assets/avatar/BossPhases(Moon)/pixil-layer-trang-ban.png");
    texBoss3Avatar = LoadTexture("Assets/avatar/BossPhases(Moon)/pixil-layer-trang-tron.png");
}

void ShutdownSetupUI() { 
    ShutdownUIFrameSystem(); 
    // Giải phóng bộ nhớ
    UnloadTexture(texBoss1Avatar);
    UnloadTexture(texBoss2Avatar);
    UnloadTexture(texBoss3Avatar);
}

void UpdateSetupUI(const MouseState &mouse, float dt, AudioAssets &audio,
                   AppSettings &settings, ScreenState &currentScreen) {
  for (int i = 0; i < gSetupButtonCount; ++i) {
    const int animIndex = 10 + i;

    bool hovered = false;
    bool pressed = false;

    UpdateUIButton(animIndex, gSetupButtons[i], mouse, dt, audio, settings,
                   hovered, pressed);

    if (hovered && mouse.leftPressed) {
      PlayMenuClick(audio, settings);

      switch (gSetupButtons[i].id) {
      case SETUP_BTN_PVE: // Người chơi bấm chuyển tab PVE
        settings.gameMode = MODE_PVE;
        break;

      case SETUP_BTN_PVP: // Người chơi bấm chuyển tab PVP
        settings.gameMode = MODE_PVP;
        break;

      case SETUP_BTN_PREV: // NÚT MŨI TÊN TRÁI (<)
        if (settings.gameMode == MODE_PVE) {
          // Ép kiểu sang int để trừ, xoay vòng từ 0 -> 2 (EASY -> HARD)
          int diff = static_cast<int>(settings.botDifficulty) - 1;
          if (diff < static_cast<int>(DIFFICULTY_EASY)) {
            diff = static_cast<int>(DIFFICULTY_HARD);
          }
          settings.botDifficulty = static_cast<BotDifficulty>(diff);
        } else if (settings.gameMode == MODE_PVP) {
          // Xoay vòng từ 0 -> 1 (CLASSIC -> TOURNAMENT)
          int mode = static_cast<int>(settings.pvpMode) - 1;
          if (mode < static_cast<int>(CLASSIC)) {
            mode = static_cast<int>(TOURNAMENT);
          }
          settings.pvpMode = static_cast<PVPMode>(mode);
        }
        break;

      case SETUP_BTN_NEXT: // NÚT MŨI TÊN PHẢI (>)
        if (settings.gameMode == MODE_PVE) {
          // Ép kiểu sang int để cộng
          int diff = static_cast<int>(settings.botDifficulty) + 1;
          if (diff > static_cast<int>(DIFFICULTY_HARD)) {
            diff = static_cast<int>(DIFFICULTY_EASY);
          }
          settings.botDifficulty = static_cast<BotDifficulty>(diff);
        } else if (settings.gameMode == MODE_PVP) {
          int mode = static_cast<int>(settings.pvpMode) + 1;
          if (mode > static_cast<int>(TOURNAMENT)) {
            mode = static_cast<int>(CLASSIC);
          }
          settings.pvpMode = static_cast<PVPMode>(mode);
        }
        break;

      case SETUP_BTN_PLAY:
        // Reset board và timer trước khi set game data mới
        ResetBoard();

        // Cập nhật chế độ chơi và độ khó từ cài đặt sang core game
        current().gameMode = settings.gameMode;
        if (settings.gameMode == MODE_PVE) {
          current().botDifficulty = settings.botDifficulty;
        } else {
          // Truyền pvpMode và timeLeft sang current() cho PVP
          current().pvpMode = static_cast<int>(settings.pvpMode);

          // Tournament mode cần có timer — nếu chưa chọn time limit thì mặc
          // định 15s
          if (settings.pvpMode == TOURNAMENT &&
              settings.timeLeft == TIME_LIMIT_NONE) {
            current().timeLeft = TIME_LIMIT_15S;
          } else {
            current().timeLeft = static_cast<int>(settings.timeLeft);
          }
        }

        // Mở màn chơi
        currentScreen = SCREEN_PLAY;
        break;

      case SETUP_BTN_BACK:
        currentScreen = SCREEN_MAIN_MENU;
        break;
      }
    }
  }
  if (IsKeyPressed(KEY_ESCAPE)) {
    currentScreen = SCREEN_MAIN_MENU;
  }
}

void DrawSetupUI(Font fontTitle, Font fontSmall, const MouseState &mouse,
                 const AppSettings &settings) {
  DrawBackgroundOnly();

  // ==========================================
  // 1. VẼ 3 CÁI KHUNG (Đã căn chỉnh lại layout)
  // ==========================================

  // Khung nền chính: Căn giữa màn hình 1600x900
  DrawPanelFrame({200.0f, 150.0f, 1200.0f, 600.0f});

  // Khung vàng (Card) chứa nội dung: trả về chiều rộng 600.0f để không đè nút
  Rectangle cardRect = {350.0f, 220.0f, 600.0f, 320.0f};
  DrawCardFrame(cardRect);

  // Khung nhỏ chứa kích thước bàn cờ (Cao 60 pixel thôi, đừng để 400 nữa nha
  // ông!)

  // ==========================================
  // 2. XỬ LÝ LOGIC CHỮ (Controller)
  // ==========================================
  const char *titleText = "";
  const char *descText = "";
  const char *detailText = "";

  if (settings.gameMode == MODE_PVE) {
    if (settings.botDifficulty == DIFFICULTY_EASY) {
      titleText = "BOSS 1";
      descText = "Crescent Moon Demon";
      detailText = "- The Demon imp with depleted magic.\n- Acts on instinct, attacks randomly.\n- Easy as pie, a simple warm-up.";
    } else if (settings.botDifficulty == DIFFICULTY_MEDIUM) {
      titleText = "BOSS 2";
      descText = "Half Moon Demon";
      detailText = "- The Demon guard with restoring magic.\n- Possesses tactical thinking and sets traps.\n- Blocks both ends, not easily fooled!";
    } else if (settings.botDifficulty == DIFFICULTY_HARD) {
      titleText = "BOSS 3";
      descText = "Full Moon Demon";
      detailText = "- The Moon Lord who kidnapped Yuki.\n- Ultimate power, manipulates the entire board.\n- Sees the future, the undefeated champion!";
    }
  } else if (settings.gameMode == MODE_PVP) {
    if (settings.pvpMode == CLASSIC) {
      titleText = "CLASSIC";
      descText = "";
      detailText =
          "- Du 5 quan lien tiep la thang\n- Khong ap dung luat chan 2 dau";
    } else if (settings.pvpMode == TOURNAMENT) {
      titleText = "TOURNAMENT";
      descText = "";
      detailText =
          "- 5 quan bi chan 2 dau khong tinh\n- Het thoi gian se bi tuoc luot";
    }
  }

  // ==========================================
  // 3. IN CHỮ ĐÈ LÊN KHUNG (Tự động căn giữa)
  // ==========================================

  if (settings.gameMode == MODE_PVE) {
      // 3A. NẾU LÀ PVE -> DÙNG GIAO DIỆN CHIA ĐÔI MÀN HÌNH "BOSS SELECT"
      BossData bData = { titleText, descText, detailText };
      Texture2D currentAvatar = texBoss1Avatar; // Tránh báo lỗi chưa khởi tạo
      float avatarScale = 3.0f; // Chỉnh thông số scale này cho vừa với ô bên trái nhé

      if (settings.botDifficulty == DIFFICULTY_EASY) currentAvatar = texBoss1Avatar;
      else if (settings.botDifficulty == DIFFICULTY_MEDIUM) currentAvatar = texBoss2Avatar;
      else if (settings.botDifficulty == DIFFICULTY_HARD) currentAvatar = texBoss3Avatar;

      DrawBossPreview(currentAvatar, cardRect, bData, avatarScale, fontTitle, fontSmall);
  }
  else {
      // 3B. NẾU LÀ PVP -> DÙNG GIAO DIỆN CHỮ CŨ (CĂN GIỮA)
      DrawCardFrame(cardRect);

      Vector2 titleSize = MeasureTextEx(fontTitle, titleText, 36.0f, 2.0f);
      float titleX = cardRect.x + (cardRect.width / 2.0f) - (titleSize.x / 2.0f);
      DrawTextEx(fontTitle, titleText, Vector2{ titleX, cardRect.y + 30.0f }, 36.0f, 2.0f, Color{ 128, 0, 32, 255 });

      Vector2 descSize = MeasureTextEx(fontSmall, descText, 24.0f, 2.0f);
      float descX = cardRect.x + (cardRect.width / 2.0f) - (descSize.x / 2.0f);
      DrawTextEx(fontSmall, descText, Vector2{ descX, cardRect.y + 80.0f }, 24.0f, 2.0f, Color{ 128, 0, 32, 255 });

      DrawTextEx(fontSmall, detailText, Vector2{ cardRect.x + 60.0f, cardRect.y + 140.0f }, 22.0f, 2.0f, Color{ 130, 40, 60, 255 });
  }

  // In kích thước bàn cờ vào Khung Nhỏ

  // ==========================================
  // --- VẼ NÚT BẤM VÀ CÁC THÀNH PHẦN KHÁC ---
  // ==========================================

  for (int i = 0; i < gSetupButtonCount; ++i) {
    const int animIndex = 10 + i;

    Rectangle hitRect = GetButtonRect(gSetupButtons[i]);
    bool hovered = IsMouseOverRect(mouse, hitRect);
    bool pressed = hovered && mouse.leftDown;

    DrawUIButton(animIndex, gSetupButtons[i], fontTitle, hovered, pressed);
  }

  DrawCenteredText(fontSmall, "PRESS ESC TO BACK", 850.0f, 20.0f,
                   Color{220, 205, 205, 180});
}









