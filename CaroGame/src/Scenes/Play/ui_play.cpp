#include "Scenes/Play/ui_play.h"
#include "Control/menu_data.h"
#include "Model/AI/ai_bot.h"
#include "Model/config.h"
#include "Model/game_data.h"
#include "Model/logic.h"
#include "Scenes/Save_Load/ui_load.h"
#include "Scenes/Save_Load/ui_save.h"
#include "Scenes/Setting/ui_setting.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include <cmath>
#include <cstdio>


static bool gPaused = false;
static const char *gPauseMessage = "";

// Thêm biến timer dành cho AI Bot (đoạn dòng 16)
static float gBotDelayTimer = 0.0f;

static Texture2D gPlayAvatarP1{};
static Texture2D gPlayAvatarP2{};
static Texture2D gPlayAvatarBot{};
static Texture2D gPieceXTex{};
static Texture2D gPieceOTex{};
static bool gPlayAssetsLoaded = false;

static constexpr float PLAY_FRAME_PAD = 14.0f;
static constexpr float PLAYER_PANEL_Y = 187.0f;
static constexpr float PLAYER_PANEL_W = 305.0f;
static constexpr float PLAYER_PANEL_H = 682.0f;
static constexpr float PLAYER_LEFT_X = 58.0f;
static constexpr float PLAYER_RIGHT_X = SCREEN_WIDTH - PLAYER_LEFT_X - PLAYER_PANEL_W;

static Rectangle GetResetButtonRect()
{
  return Rectangle{1282.0f, 42.0f, 164.0f, 50.0f};
}

static bool IsTextureReadyEx(const Texture2D &tex)
{
  return tex.id != 0;
}

static Texture2D LoadTextureFromCandidates(const char *a, const char *b = nullptr)
{
  if (a && FileExists(a))
    return LoadTexture(a);
  if (b && FileExists(b))
    return LoadTexture(b);
  return Texture2D{};
}

static void LoadPlayAssets()
{
  if (gPlayAssetsLoaded)
    return;

  gPlayAvatarP1 = LoadTextureFromCandidates("assets/avatars/character-1.png", "Assets/avatars/character-1.png");
  gPlayAvatarP2 = LoadTextureFromCandidates("assets/avatars/character-2.png", "Assets/avatars/character-2.png");
  gPlayAvatarBot = LoadTextureFromCandidates("assets/avatars/boss.png", "Assets/avatars/boss.png");
  gPieceXTex = LoadTextureFromCandidates("assets/X-O/pixil-layer-2.png", "Assets/X-O/pixil-layer-2.png");
  gPieceOTex = LoadTextureFromCandidates("assets/X-O/pixil-layer-1.png", "Assets/X-O/pixil-layer-1.png");
  gPlayAssetsLoaded = true;
}

static void UnloadPlayAssets()
{
  if (IsTextureReadyEx(gPlayAvatarP1))
    UnloadTexture(gPlayAvatarP1);
  if (IsTextureReadyEx(gPlayAvatarP2))
    UnloadTexture(gPlayAvatarP2);
  if (IsTextureReadyEx(gPlayAvatarBot))
    UnloadTexture(gPlayAvatarBot);
  if (IsTextureReadyEx(gPieceXTex))
    UnloadTexture(gPieceXTex);
  if (IsTextureReadyEx(gPieceOTex))
    UnloadTexture(gPieceOTex);

  gPlayAvatarP1 = {};
  gPlayAvatarP2 = {};
  gPlayAvatarBot = {};
  gPieceXTex = {};
  gPieceOTex = {};
  gPlayAssetsLoaded = false;
}

// ─── Board helpers ────────────────────────────────────────────────────
static Rectangle GetBoardRect() {
  return {BOARD_START_X, BOARD_START_Y, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE};
}

static bool IsInsideBoard(Vector2 pos) {
  Rectangle r = GetBoardRect();
  return pos.x >= r.x && pos.x <= r.x + r.width && pos.y >= r.y &&
         pos.y <= r.y + r.height;
}

static bool GetCellFromMouse(Vector2 pos, int &outRow, int &outCol) {
  if (!IsInsideBoard(pos))
    return false;
  outCol = static_cast<int>((pos.x - BOARD_START_X) / CELL_SIZE);
  outRow = static_cast<int>((pos.y - BOARD_START_Y) / CELL_SIZE);
  if (outRow < 0 || outRow >= BOARD_SIZE || outCol < 0 || outCol >= BOARD_SIZE)
    return false;
  return true;
}

static void UpdateGameStateAfterMove() {
  current().result = TestBoard(current().lastMoveRow, current().lastMoveCol);
  if (current().result == RESULT_ONGOING)
    if (IsBoardFull()) {
      current().result = RESULT_DRAW;
    }
}

// ─── Draw helpers ─────────────────────────────────────────────────────
static void DrawCenteredText(Font font, const char *text, float y,
                             float fontSize, Color color) {
  Vector2 size = MeasureTextEx(font, text, fontSize, 1.0f);
  DrawTextEx(font, text, Vector2{SCREEN_WIDTH * 0.5f - size.x * 0.5f, y},
             fontSize, 1.0f, color);
}

static void DrawTextCenteredInRect(Font font, const char *text, Rectangle rect,
                                   float fontSize, Color color,
                                   float spacing = 1.0f) {
  Vector2 size = MeasureTextEx(font, text, fontSize, spacing);
  DrawTextEx(font, text,
             Vector2{rect.x + rect.width * 0.5f - size.x * 0.5f,
                     rect.y + rect.height * 0.5f - size.y * 0.5f},
             fontSize, spacing, color);
}

static void DrawHeaderStrip(Rectangle rect, Color fill) {
  DrawRectangleRounded(rect, 0.08f, 8, Color{92, 28, 34, 245});
  Rectangle inner{rect.x + 10.0f, rect.y + 8.0f, rect.width - 20.0f,
                  rect.height - 16.0f};
  DrawRectangleRounded(inner, 0.06f, 8, fill);
  DrawRectangleRoundedLinesEx(inner, 0.06f, 8, 2.0f, Color{104, 31, 35, 220});
}

static void DrawFramedAvatar(const Texture2D &tex, Rectangle rect) {
  DrawCardFrame(rect);
  DrawRectangleRounded(Rectangle{rect.x + 8.0f, rect.y + 8.0f, rect.width - 16.0f, rect.height - 16.0f},
                       0.04f, 5, Color{206, 218, 170, 210});

  if (!IsTextureReadyEx(tex))
    return;

  const float pad = 14.0f;
  Rectangle dest{rect.x + pad, rect.y + pad, rect.width - pad * 2.0f,
                 rect.height - pad * 2.0f};
  DrawTexturePro(tex,
                 Rectangle{0.0f, 0.0f, static_cast<float>(tex.width),
                           static_cast<float>(tex.height)},
                 dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
}

static const char *GetPlayerName(bool playerOne) {
  const char *name = playerOne ? current().namePlayer1 : current().namePlayer2;
  if (name && name[0] != '\0')
    return name;
  return playerOne ? "PLAYER 1" : (current().gameMode == MODE_PVE ? "BOT" : "PLAYER 2");
}

static void DrawStatRow(Font fontSmall, Rectangle row, const char *label,
                        const char *value, Color valueColor) {
  DrawSmallFrame(row);
  DrawRectangleRounded(Rectangle{row.x + 5.0f, row.y + 5.0f, row.width - 10.0f, row.height - 10.0f},
                       0.08f, 6, Color{40, 32, 23, 130});
  DrawTextEx(fontSmall, label, Vector2{row.x + 56.0f, row.y + 15.0f}, 21.0f, 1.0f,
             Color{255, 236, 194, 240});
  Vector2 valueSize = MeasureTextEx(fontSmall, value, 21.0f, 1.0f);
  DrawTextEx(fontSmall, value,
             Vector2{row.x + row.width - valueSize.x - 20.0f, row.y + 15.0f},
             21.0f, 1.0f, valueColor);
}

static void DrawPlayerPanel(Font fontTitle, Font fontSmall, bool playerOne) {
  const float x = playerOne ? PLAYER_LEFT_X : PLAYER_RIGHT_X;
  Rectangle panel{x, PLAYER_PANEL_Y, PLAYER_PANEL_W, PLAYER_PANEL_H};
  DrawPanelFrame(panel);

  Rectangle header{x + 22.0f, panel.y + 20.0f, PLAYER_PANEL_W - 44.0f, 58.0f};
  DrawRectangleRounded(header, 0.08f, 8, Color{172, 42, 55, 235});
  DrawRectangleLinesEx(Rectangle{header.x + 4.0f, header.y + header.height - 1.0f,
                                 header.width - 8.0f, 1.0f},
                       2.0f, Color{98, 28, 32, 220});
  DrawTextCenteredInRect(fontSmall, playerOne ? "PLAYER 1" : "PLAYER 2", header,
                         24.0f, Color{255, 243, 211, 255}, 2.0f);

  const Texture2D &avatar = playerOne ? gPlayAvatarP1
                                      : (current().gameMode == MODE_PVE ? gPlayAvatarBot : gPlayAvatarP2);
  DrawFramedAvatar(avatar, Rectangle{x + 66.0f, panel.y + 92.0f, 172.0f, 172.0f});

  const bool active = current().result == RESULT_ONGOING &&
                      ((playerOne && current().turn == CELL_X) ||
                       (!playerOne && current().turn == CELL_O));
  DrawTextCenteredInRect(fontTitle, playerOne ? "X TEAM" : "O TEAM",
                         Rectangle{x + 52.0f, panel.y + 284.0f, 202.0f, 46.0f},
                         30.0f,
                         playerOne ? Color{255, 92, 98, 255} : Color{102, 178, 255, 255},
                         1.0f);

  char wins[16];
  char moves[16];
  snprintf(wins, sizeof(wins), "%d", playerOne ? current().scorePlayer1 : current().scorePlayer2);

  int moveCount = 0;
  const int piece = playerOne ? CELL_X : CELL_O;
  for (int r = 0; r < BOARD_SIZE; ++r)
    for (int c = 0; c < BOARD_SIZE; ++c)
      if (current().board[r][c] == piece)
        ++moveCount;
  snprintf(moves, sizeof(moves), "%d", moveCount);

  const float rowX = x + 20.0f;
  DrawStatRow(fontSmall, Rectangle{rowX, panel.y + 342.0f, 265.0f, 55.0f}, "Wins", wins,
              playerOne ? Color{255, 92, 98, 255} : Color{102, 178, 255, 255});
  DrawStatRow(fontSmall, Rectangle{rowX, panel.y + 407.0f, 265.0f, 55.0f}, "Moves", moves,
              playerOne ? Color{255, 92, 98, 255} : Color{102, 178, 255, 255});
  DrawStatRow(fontSmall, Rectangle{rowX, panel.y + 472.0f, 265.0f, 55.0f}, "Status",
              active ? "Turn" : (current().result == RESULT_ONGOING ? "Ready" : "Done"),
              active ? Color{255, 214, 84, 255} : Color{255, 236, 194, 230});

  Button stateButton{
      Vector2{x + 40.0f, panel.y + 590.0f},
      Vector2{225.0f, 58.0f},
      active ? "YOUR TURN" : "WAITING",
      0,
      BUTTON_VISUAL_TEXT,
      BUTTON_ICON_NONE,
      22.0f,
      1.0f};
  DrawUIButton(playerOne ? 70 : 71, stateButton, fontSmall, active, false);
}

static void DrawPlayTopBar(Font fontTitle, Font fontSmall, const MouseState &mouse) {
  Rectangle topBar{32.0f, 28.0f, SCREEN_WIDTH - 64.0f, 78.0f};
  DrawHeaderStrip(topBar, Color{170, 43, 56, 235});

  DrawTextEx(fontTitle, "CARO", Vector2{82.0f, 52.0f}, 30.0f, 3.0f,
             Color{255, 214, 94, 255});

  Rectangle center{318.0f, 32.0f, 900.0f, 68.0f};
  DrawSmallFrame(center);
  DrawRectangleRounded(Rectangle{center.x + 14.0f, center.y + 12.0f,
                                 center.width - 28.0f, center.height - 24.0f},
                       0.05f, 6, Color{45, 35, 25, 150});

  const char *turnText = "TURN: X";
  Color turnColor{255, 92, 98, 255};
  if (current().result == RESULT_X_WINS) {
    turnText = "WINNER: X";
  } else if (current().result == RESULT_O_WINS) {
    turnText = "WINNER: O";
    turnColor = Color{102, 178, 255, 255};
  } else if (current().result == RESULT_DRAW) {
    turnText = "DRAW";
    turnColor = Color{255, 214, 84, 255};
  } else if (current().turn == CELL_O) {
    turnText = "TURN: O";
    turnColor = Color{102, 178, 255, 255};
  }

  DrawTextCenteredInRect(fontSmall, turnText,
                         Rectangle{center.x, center.y + 9.0f, center.width, 22.0f},
                         18.0f, Color{255, 243, 211, 255}, 2.0f);
  DrawCircleV(Vector2{center.x + center.width * 0.5f - 76.0f, center.y + 22.0f}, 5.0f, turnColor);
  DrawCircleV(Vector2{center.x + center.width * 0.5f + 76.0f, center.y + 22.0f}, 5.0f, turnColor);
  DrawTextCenteredInRect(fontSmall, "5 IN A ROW TO WIN",
                         Rectangle{center.x, center.y + 39.0f, center.width, 22.0f},
                         16.0f, Color{255, 236, 194, 245}, 2.0f);

  Button resetButton{
      Vector2{GetResetButtonRect().x, GetResetButtonRect().y},
      Vector2{GetResetButtonRect().width, GetResetButtonRect().height},
      "RESET",
      0,
      BUTTON_VISUAL_TEXT,
      BUTTON_ICON_NONE,
      22.0f,
      1.0f};
  const Rectangle hit = GetResetButtonRect();
  const bool hovered = IsMouseOverRect(mouse, hit);
  DrawUIButton(72, resetButton, fontSmall, hovered, hovered && mouse.leftDown);
}

static void DrawBoardTitle(Font fontTitle) {
  Rectangle badge{SCREEN_WIDTH * 0.5f - 205.0f, 164.0f, 410.0f, 56.0f};
  DrawSmallFrame(badge);
  DrawTextCenteredInRect(fontTitle, "BATTLE BOARD", badge, 28.0f,
                         Color{255, 214, 94, 255}, 2.0f);
}

static void DrawBottomHint(Font fontSmall) {
  Rectangle hint{SCREEN_WIDTH * 0.5f - 300.0f, 914.0f, 600.0f, 72.0f};
  (void)hint;
  DrawSmallFrame(Rectangle{SCREEN_WIDTH * 0.5f - 300.0f, 842.0f, 600.0f, 52.0f});
  DrawTextEx(fontSmall, "i", Vector2{SCREEN_WIDTH * 0.5f - 280.0f, 854.0f}, 24.0f, 1.0f,
             Color{255, 214, 94, 255});
  DrawTextEx(fontSmall, "The first player to get 5 in a row wins.",
             Vector2{SCREEN_WIDTH * 0.5f - 225.0f, 856.0f}, 21.0f, 1.0f,
             Color{255, 236, 194, 240});
}

static void DrawBoardGrid(const AppSettings &settings) {
  Rectangle boardRect = GetBoardRect();
  Rectangle frameRect{boardRect.x - PLAY_FRAME_PAD, boardRect.y - PLAY_FRAME_PAD,
                      boardRect.width + PLAY_FRAME_PAD * 2.0f,
                      boardRect.height + PLAY_FRAME_PAD * 2.0f};
  DrawCardFrame(frameRect);

  unsigned char lineAlpha = 150;

  for (int i = 1; i < BOARD_SIZE; ++i) {
    float x = BOARD_START_X + i * CELL_SIZE;
    float y = BOARD_START_Y + i * CELL_SIZE;

    DrawLineEx({x, BOARD_START_Y}, {x, BOARD_START_Y + BOARD_PIXEL_SIZE}, 2.0f,
               Color{72, 56, 38, lineAlpha});
    DrawLineEx({BOARD_START_X, y}, {BOARD_START_X + BOARD_PIXEL_SIZE, y}, 2.0f,
               Color{72, 56, 38, lineAlpha});
  }

  DrawRectangleLinesEx(boardRect, 2.0f, Color{70, 50, 34, 170});

  const int starCells[5][2] = {{4, 4}, {4, 10}, {7, 7}, {10, 4}, {10, 10}};
  for (int i = 0; i < 5; ++i) {
    const float cx = BOARD_START_X + starCells[i][1] * CELL_SIZE;
    const float cy = BOARD_START_Y + starCells[i][0] * CELL_SIZE;
    DrawCircleV(Vector2{cx, cy}, 4.5f, Color{68, 49, 35, 210});
  }
}

// Draws a soft highlight on the last placed cell
static void DrawHighlightCell(const AppSettings &settings) {
  if (!settings.highlightLastMove || current().lastMoveRow < 0)
    return;

  float x = BOARD_START_X + current().lastMoveCol * CELL_SIZE + 1.0f;
  float y = BOARD_START_Y + current().lastMoveRow * CELL_SIZE + 1.0f;
  float s = (float)CELL_SIZE - 2.0f;

  DrawRectangle((int)x, (int)y, (int)s, (int)s, Color{255, 245, 160, 35});
  DrawRectangleLinesEx({x, y, s, s}, 2.0f, Color{255, 230, 80, 140});
}

static void DrawWinLine(const AppSettings &settings) {
  if (current().result == RESULT_ONGOING || current().winLineCount <= 0)
    return;
  Color col = (current().result == CELL_X) ? Color{120, 220, 255, 220}
                                           : Color{255, 150, 200, 220};

  int count = current().winLineCount;

  // Highlight each winning cell (dùng winLineCount thay vì WIN_LENGTH để hỗ trợ
  // chuỗi > 5)
  for (int i = 0; i < count; ++i) {
    float x = BOARD_START_X + current().winLine[i][1] * CELL_SIZE + 1.0f;
    float y = BOARD_START_Y + current().winLine[i][0] * CELL_SIZE + 1.0f;
    float s = (float)CELL_SIZE - 2.0f;
    DrawRectangle((int)x, (int)y, (int)s, (int)s,
                  Color{col.r, col.g, col.b, (unsigned char)(col.a / 3)});
  }

  // Thick line from center of first to last winning cell
  float x1 =
      BOARD_START_X + current().winLine[0][1] * CELL_SIZE + CELL_SIZE * 0.5f;
  float y1 =
      BOARD_START_Y + current().winLine[0][0] * CELL_SIZE + CELL_SIZE * 0.5f;
  float x2 = BOARD_START_X + current().winLine[count - 1][1] * CELL_SIZE +
             CELL_SIZE * 0.5f;
  float y2 = BOARD_START_Y + current().winLine[count - 1][0] * CELL_SIZE +
             CELL_SIZE * 0.5f;
  DrawLineEx({x1, y1}, {x2, y2}, 5.0f, col);
}

static void DrawPieces(Font fontTitle, const AppSettings &settings) {
  for (int r = 0; r < BOARD_SIZE; ++r) {
    for (int c = 0; c < BOARD_SIZE; ++c) {
      if (current().board[r][c] == CELL_EMPTY)
        continue;

      const char *text = (current().board[r][c] == CELL_X) ? "X" : "O";
      Color color = (current().board[r][c] == CELL_X)
                        ? Color{120, 220, 255, 255}
                        : Color{255, 150, 200, 255};
      const Texture2D &pieceTex =
          (current().board[r][c] == CELL_X) ? gPieceXTex : gPieceOTex;

      Rectangle cellRect{BOARD_START_X + c * CELL_SIZE,
                         BOARD_START_Y + r * CELL_SIZE, (float)CELL_SIZE,
                         (float)CELL_SIZE};

      if (IsTextureReadyEx(pieceTex)) {
        const float pad = 7.0f;
        Rectangle dest{cellRect.x + pad, cellRect.y + pad,
                       cellRect.width - pad * 2.0f,
                       cellRect.height - pad * 2.0f};
        DrawTexturePro(pieceTex,
                       Rectangle{0.0f, 0.0f, static_cast<float>(pieceTex.width),
                                 static_cast<float>(pieceTex.height)},
                       dest, Vector2{0.0f, 0.0f}, 0.0f, WHITE);
        continue;
      }

      Vector2 size = MeasureTextEx(fontTitle, text, 38.0f, 2.0f);
      Vector2 pos{cellRect.x + cellRect.width * 0.5f - size.x * 0.5f,
                  cellRect.y + cellRect.height * 0.5f - size.y * 0.5f};

      DrawTextEx(fontTitle, text, pos, 38.0f, 2.0f, color);
    }
  }
}

// ─── Pause ────────────────────────────────────────────────────────────
static void UpdatePauseMenu(const MouseState &mouse, float dt,
                            AudioAssets &audio, const AppSettings &settings,
                            ScreenState &currentScreen, bool &shouldClose) {
  for (int i = 0; i < gPauseButtonCount; ++i) {
    const int animIndex = 20 + i;
    bool hovered = false, pressed = false;
    UpdateUIButton(animIndex, gPauseButtons[i], mouse, dt, audio, settings,
                   hovered, pressed);

    if (hovered && mouse.leftPressed) {
      PlayMenuClick(audio, settings);
      switch (gPauseButtons[i].id) {
      case PAUSE_BTN_CONTINUE:
        gPaused = false;
        gPauseMessage = "";
        break;

      case PAUSE_BTN_SETTING:
        SetSettingReturnScreen(SCREEN_PLAY); // Back sẽ quay về Play (còn pause)
        currentScreen = SCREEN_SETTING;
        // gPaused giữ nguyên true — khi Back về Play sẽ thấy lại pause menu
        gPauseMessage = "";
        break;

      case PAUSE_BTN_SAVE:
        if (current().nameGame[0] == '\0') {
          currentScreen = SCREEN_SAVE_FIRST;
        } else {
          currentScreen = SCREEN_SAVE_SECOND;
        }
        break;

      case PAUSE_BTN_SAVE_AS:
        if (current().nameGame[0] == '\0') {
          currentScreen = SCREEN_SAVE_FIRST;
        } else {
          InitSaveUI(); // Reset trạng thái UI Save As trước khi vào
          currentScreen = SCREEN_SAVE_AS;
        }
        break;

      case PAUSE_BTN_EXIT_MENU:
        gPaused = false;
        gPauseMessage = "";
        currentScreen = SCREEN_NOTIFY_BACK_MENU;
        break;

      case PAUSE_BTN_EXIT_DESKTOP:
        gPaused = false;
        gPauseMessage = "";
        currentScreen = SCREEN_NOTIFY_EXIT;
        break;
      }
    }
  }
}

static void DrawPauseOverlay(Font fontTitle, Font fontSmall,
                             const MouseState &mouse) {
  DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{0, 0, 0, 150});

  Rectangle panel{SCREEN_WIDTH * 0.5f - 240.0f, SCREEN_HEIGHT * 0.5f - 250.0f,
                  480.0f, 500.0f};

  DrawPanelFrame(panel);
  DrawCenteredText(fontTitle, "PAUSE", panel.y + 32.0f, 36.0f,
                   Color{255, 235, 225, 255});

  for (int i = 0; i < gPauseButtonCount; ++i) {
    const int animIndex = 20 + i;
    Rectangle hitRect = GetButtonRect(gPauseButtons[i]);
    bool hovered = IsMouseOverRect(mouse, hitRect);
    bool pressed = hovered && mouse.leftDown;
    DrawUIButton(animIndex, gPauseButtons[i], fontSmall, hovered, pressed);
  }

  if (gPauseMessage && gPauseMessage[0] != '\0') {
    DrawCenteredText(fontSmall, gPauseMessage, panel.y + panel.height - 44.0f,
                     20.0f, Color{255, 210, 210, 220});
  }
}

// ─── Public API ───────────────────────────────────────────────────────
void InitPlayUI() {
  LoadPlayAssets();
  ResetBoard(); // Tạm thời có thể gọi hàm này để khởi tạo game data.
                // Sau này có thể gọi hàm khác nếu cần thiết
}
void ShutdownPlayUI() { UnloadPlayAssets(); }

void UpdatePlayUI(const MouseState &mouse, float dt, AudioAssets &audio,
                  const AppSettings &settings, ScreenState &currentScreen,
                  bool &shouldClose) {
  if (IsKeyPressed(KEY_R)) {
    ResetBoard();
    return;
  }

  if (mouse.leftPressed && IsMouseOverRect(mouse, GetResetButtonRect())) {
    PlayMenuClick(audio, settings);
    ResetBoard();
    gBotDelayTimer = 0.0f;
    return;
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    gPaused = !gPaused;
    gPauseMessage = "";
    return;
  }

  if (gPaused) {
    UpdatePauseMenu(mouse, dt, audio, settings, currentScreen, shouldClose);
    return;
  }

  // Nếu game đã có kết quả thắng hòa thì không làm gì thêm
  if (current().result != RESULT_ONGOING)
    return;

  // --------- CẬP NHẬT TIMER (Tournament / PVE) ---------
  int timerSignal = UpdateTimer();
  // Nếu hết giờ → turn đã được đổi bên trong UpdateTimer()
  // Skip input handling trong frame này để tránh double-flip turn
  if (timerSignal == -99)
    return;

  // --------- TÍCH HỢP PVE & PVP ---------

  // Kiểm tra xem hiện tại đang ở chế độ PVE và đang là lượt của BOT (CELL_O)
  // hay không
  bool isBotTurn = (current().gameMode == MODE_PVE && current().turn == CELL_O);

  if (isBotTurn) {
    // [NHÁNH 1: LƯỢT CỦA AI BOTS]
    gBotDelayTimer += dt; // Cộng dồn delta time (khoảng cách giữa các frame)

    if (gBotDelayTimer >= 0.5f) // Trễ 0.5 giây mới để AI đánh
    {
      // Gọi logic của Bot.
      // LƯU Ý: Vì bạn nói BotMakeMove() đã tự đánh xuống, tự đổi turn(CELL_X),
      // nên bạn chỉ cần gọi trực tiếp nó ở đây.
      BotMakeMove();

      // Cập nhật trạng thái thắng/thua sau nước đi của Bot
      UpdateGameStateAfterMove();

      if (current().result == RESULT_ONGOING) {
        current().turn = (current().turn == CELL_X) ? CELL_O : CELL_X;
      }

      // Đặt lại số đếm thời gian cho lượt đánh sau
      gBotDelayTimer = 0.0f;
    }
  } else {
    // [NHÁNH 2: LƯỢT CỦA NGƯỜI CHƠI - BLOCK MOUSE KHỎI BOT]
    // Vì ta đặt trong cụm `else`, nên nếu là isBotTurn == true, click chuột sẽ
    // toàn hoàn bị vô hiệu hóa!
    if (mouse.leftPressed) {
      // Dùng CheckBoard() để đặt quân — tránh duplicate logic
      // CheckBoard() đã xử lý: ghi vào board, cập nhật lastMoveRow/Col, reset
      // timer
      int placed = CheckBoard((int)mouse.position.x, (int)mouse.position.y);

      if (placed != 0) {
        UpdateGameStateAfterMove();

        if (current().result == RESULT_ONGOING) {
          current().turn = (current().turn == CELL_X) ? CELL_O : CELL_X;
          // Reset lại timer để chắc chắn timer bắt đầu từ độ trễ 0s ở lượt tới
          gBotDelayTimer = 0.0f;
        }
      }
    }
  }
}

void DrawPlayUI(Font fontTitle, Font fontSmall, const MouseState &mouse,
                const AppSettings &settings) {
  DrawBackgroundOnly();
  LoadPlayAssets();
  DrawPlayTopBar(fontTitle, fontSmall, mouse);
  DrawBoardTitle(fontTitle);
  DrawPlayerPanel(fontTitle, fontSmall, true);
  DrawPlayerPanel(fontTitle, fontSmall, false);

  // --- HIỂN THỊ COUNTDOWN TIMER (Tournament / PVE) ---
  float timeLeft = GetTimeLeft();
  if (timeLeft >= 0.0f && current().result == RESULT_ONGOING) {
    char timerBuf[32];
    int seconds =
        (int)ceilf(timeLeft); // Làm tròn lên để không hiển thị 0 khi còn < 1s
    snprintf(timerBuf, sizeof(timerBuf), "TIME: %ds", seconds);

    // Đổi màu cảnh báo khi sắp hết giờ
    Color timerColor = (timeLeft <= 3.0f)
                           ? Color{255, 80, 80, 255}    // Đỏ cảnh báo
                           : Color{255, 230, 160, 230}; // Vàng nhạt bình thường

    DrawTextCenteredInRect(fontSmall, timerBuf,
                           Rectangle{BOARD_START_X + BOARD_PIXEL_SIZE - 126.0f,
                                     BOARD_START_Y - 58.0f, 128.0f, 36.0f},
                           20.0f, timerColor, 1.0f);
  }

  DrawBoardGrid(settings);
  DrawHighlightCell(settings);
  DrawWinLine(settings);
  DrawPieces(fontTitle, settings);
  DrawBottomHint(fontSmall);

  if (gPaused)
    DrawPauseOverlay(fontTitle, fontSmall, mouse);
}
