#include "Scenes/Play/ui_play.h"
#include "Scenes/Setting/ui_setting.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/config.h"
#include "Control/menu_data.h"
#include "Model/Logic.h"
#include "Model/game_data.h"
#include "Scenes/Save_Load/ui_save.h"
#include "Scenes/Save_Load/ui_load.h"
#include "Model/ai_bot.h"
#include <cmath>

static bool gPaused = false;
static const char* gPauseMessage = "";

// Thêm biến timer dành cho AI Bot (đoạn dòng 16)
static float gBotDelayTimer = 0.0f; 


// Winning cells (filled when gWinner != CELL_EMPTY)
static int gWinCells[WIN_LENGTH][2] = {};

// ─── Board helpers ────────────────────────────────────────────────────
static Rectangle GetBoardRect()
{
    return { BOARD_START_X, BOARD_START_Y, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE };
}

static bool IsInsideBoard(Vector2 pos)
{
    Rectangle r = GetBoardRect();
    return pos.x >= r.x && pos.x <= r.x + r.width &&
        pos.y >= r.y && pos.y <= r.y + r.height;
}

static bool GetCellFromMouse(Vector2 pos, int& outRow, int& outCol)
{
    if (!IsInsideBoard(pos)) return false;
    outCol = static_cast<int>((pos.x - BOARD_START_X) / CELL_SIZE);
    outRow = static_cast<int>((pos.y - BOARD_START_Y) / CELL_SIZE);
    if (outRow < 0 || outRow >= BOARD_SIZE || outCol < 0 || outCol >= BOARD_SIZE) return false;
    return true;
}

static void UpdateGameStateAfterMove()
{
    current().result = TestBoard(current().lastMoveRow, current().lastMoveCol, gWinCells);
    if (current().result == CELL_EMPTY)
        if (IsBoardFull())
        {
            current().result = RESULT_DRAW;
        }
}

// ─── Draw helpers ─────────────────────────────────────────────────────
static void DrawCenteredText(Font font, const char* text, float y, float fontSize, Color color)
{
    Vector2 size = MeasureTextEx(font, text, fontSize, 1.0f);
    DrawTextEx(font, text,
        Vector2{ SCREEN_WIDTH * 0.5f - size.x * 0.5f, y },
        fontSize, 1.0f, color);
}

static void DrawBoardGrid(const AppSettings& settings)
{
    Rectangle boardRect = GetBoardRect();
    DrawRectangleRounded(boardRect, 0.02f, 8, Color{ 18, 18, 30, 150 });
    DrawRectangleLinesEx(boardRect, 3.0f, Color{ 220, 230, 255, 220 });

    unsigned char lineAlpha = 190;

    for (int i = 1; i < BOARD_SIZE; ++i)
    {
        float x = BOARD_START_X + i * CELL_SIZE;
        float y = BOARD_START_Y + i * CELL_SIZE;

        DrawLineEx({ x, BOARD_START_Y }, { x, BOARD_START_Y + BOARD_PIXEL_SIZE },
            2.0f, Color{ 180, 200, 255, lineAlpha });
        DrawLineEx({ BOARD_START_X, y }, { BOARD_START_X + BOARD_PIXEL_SIZE, y },
            2.0f, Color{ 180, 200, 255, lineAlpha });
    }
}

// Draws a soft highlight on the last placed cell
static void DrawHighlightCell(const AppSettings& settings)
{
    if (!settings.highlightLastMove || current().lastMoveRow < 0) return;

    float x = BOARD_START_X + current().lastMoveCol * CELL_SIZE + 1.0f;
    float y = BOARD_START_Y + current().lastMoveRow * CELL_SIZE + 1.0f;
    float s = (float)CELL_SIZE - 2.0f;

    DrawRectangle((int)x, (int)y, (int)s, (int)s, Color{ 255, 245, 160, 35 });
    DrawRectangleLinesEx({ x, y, s, s }, 2.0f, Color{ 255, 230, 80, 140 });
}

static void DrawWinLine(const AppSettings& settings)
{
    if (current().result == CELL_EMPTY) return;
    Color col = (current().result == CELL_X)
        ? Color{ 120, 220, 255, 220 }
    : Color{ 255, 150, 200, 220 };


    // Highlight each winning cell
    for (int i = 0; i < WIN_LENGTH; ++i)
    {
        float x = BOARD_START_X + gWinCells[i][1] * CELL_SIZE + 1.0f;
        float y = BOARD_START_Y + gWinCells[i][0] * CELL_SIZE + 1.0f;
        float s = (float)CELL_SIZE - 2.0f;
        DrawRectangle((int)x, (int)y, (int)s, (int)s,
            Color{ col.r, col.g, col.b, (unsigned char)(col.a / 3) });
    }

    // Thick line from center of first to last winning cell
    float x1 = BOARD_START_X + gWinCells[0][1] * CELL_SIZE + CELL_SIZE * 0.5f;
    float y1 = BOARD_START_Y + gWinCells[0][0] * CELL_SIZE + CELL_SIZE * 0.5f;
    float x2 = BOARD_START_X + gWinCells[WIN_LENGTH - 1][1] * CELL_SIZE + CELL_SIZE * 0.5f;
    float y2 = BOARD_START_Y + gWinCells[WIN_LENGTH - 1][0] * CELL_SIZE + CELL_SIZE * 0.5f;
    DrawLineEx({ x1, y1 }, { x2, y2 }, 5.0f, col);
}

static void DrawPieces(Font fontTitle, const AppSettings& settings)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (current().board[r][c] == CELL_EMPTY) continue;

            const char* text = (current().board[r][c] == CELL_X) ? "X" : "O";
            Color color = (current().board[r][c] == CELL_X)
                ? Color{ 120, 220, 255, 255 }
            : Color{ 255, 150, 200, 255 };

            float cx = BOARD_START_X + c * CELL_SIZE + CELL_SIZE * 0.5f;
            float cy = BOARD_START_Y + r * CELL_SIZE + CELL_SIZE * 0.5f;

            Rectangle cellRect{
                BOARD_START_X + c * CELL_SIZE,
                BOARD_START_Y + r * CELL_SIZE,
                (float)CELL_SIZE,
                (float)CELL_SIZE
            };

            Vector2 size = MeasureTextEx(fontTitle, text, 38.0f, 2.0f);
            Vector2 pos{
                cellRect.x + cellRect.width * 0.5f - size.x * 0.5f,
                cellRect.y + cellRect.height * 0.5f - size.y * 0.5f
            };

            DrawTextEx(fontTitle, text, pos, 38.0f, 2.0f, color);
        }
    }
}

// ─── Pause ────────────────────────────────────────────────────────────
static void UpdatePauseMenu(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
)
{
    for (int i = 0; i < gPauseButtonCount; ++i)
    {
        const int animIndex = 20 + i;
        bool hovered = false, pressed = false;
        UpdateUIButton(animIndex, gPauseButtons[i], mouse, dt, audio, settings, hovered, pressed);

        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);
            switch (gPauseButtons[i].id)
            {
            case PAUSE_BTN_CONTINUE:
                gPaused = false;
                gPauseMessage = "";
                break;
            case PAUSE_BTN_SETTING:
                SetSettingReturnScreen(SCREEN_PLAY);  // Back sẽ quay về Play (còn pause)
                currentScreen = SCREEN_SETTING;
                // gPaused giữ nguyên true — khi Back về Play sẽ thấy lại pause menu
                gPauseMessage = "";
                break;
            case PAUSE_BTN_SAVE:
                if (current().nameGame[0] == '\0') {
                    currentScreen = SCREEN_SAVE_FIRST;
                }
                else {
                    currentScreen = SCREEN_SAVE_SECOND;
                }
                break;
            case PAUSE_BTN_SAVE_AS:
                if (current().nameGame[0] == '\0') {
                    currentScreen = SCREEN_SAVE_FIRST;
                }
                else {
                    InitSaveUI(); // Reset trạng thái UI Save As trước khi vào
                    currentScreen = SCREEN_SAVE_AS;
                }
                break;
            case PAUSE_BTN_LOAD:
                InitLoadUI(); // Reset trạng thái UI Load trước khi vào
                currentScreen = SCREEN_LOAD;
                break;
            case PAUSE_BTN_EXIT_MENU:
                gPaused = false;
                gPauseMessage = "";
                currentScreen = SCREEN_MAIN_MENU;
                break;
            case PAUSE_BTN_EXIT_DESKTOP:
                gPaused = false;
                gPauseMessage = "";
                shouldClose = true;
                break;
            }
        }
    }
}

static void DrawPauseOverlay(Font fontTitle, Font fontSmall, const MouseState& mouse)
{
    DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color{ 0, 0, 0, 150 });

    Rectangle panel{
        SCREEN_WIDTH * 0.5f - 240.0f,
        SCREEN_HEIGHT * 0.5f - 250.0f,
        480.0f,
        500.0f
    };

    DrawPanelFrame(panel);
    DrawCenteredText(fontTitle, "PAUSE", panel.y + 32.0f, 36.0f, Color{ 255, 235, 225, 255 });

    for (int i = 0; i < gPauseButtonCount; ++i)
    {
        const int animIndex = 20 + i;
        Rectangle hitRect = GetButtonRect(gPauseButtons[i]);
        bool hovered = IsMouseOverRect(mouse, hitRect);
        bool pressed = hovered && mouse.leftDown;
        DrawUIButton(animIndex, gPauseButtons[i], fontSmall, hovered, pressed);
    }

    if (gPauseMessage && gPauseMessage[0] != '\0')
    {
        DrawCenteredText(fontSmall, gPauseMessage,
            panel.y + panel.height - 44.0f, 20.0f, Color{ 255, 210, 210, 220 });
    }
}

// ─── Public API ───────────────────────────────────────────────────────
void InitPlayUI() {
    ResetBoard(); // Tạm thời có thể gọi hàm này để khởi tạo game data.
    // Sau này có thể gọi hàm khác nếu cần thiết
}
void ShutdownPlayUI() {}

void UpdatePlayUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
)
{
    if (IsKeyPressed(KEY_R)) { ResetBoard(); return; }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        gPaused = !gPaused;
        gPauseMessage = "";
        return;
    }

    if (gPaused)
    {
        UpdatePauseMenu(mouse, dt, audio, settings, currentScreen, shouldClose);
        return;
    }

    // Nếu game đã có kết quả thắng hòa thì không làm gì thêm
    if (current().result != CELL_EMPTY || current().result == RESULT_DRAW) return;

    // --------- TÍCH HỢP PVE & PVP ---------

    // Kiểm tra xem hiện tại đang ở chế độ PVE và đang là lượt của BOT (CELL_O) hay không
    bool isBotTurn = (current().gameMode == MODE_PVE && current().turn == CELL_O);

    if (isBotTurn)
    {
        // [NHÁNH 1: LƯỢT CỦA AI BOTS]
        gBotDelayTimer += dt; // Cộng dồn delta time (khoảng cách giữa các frame)

        if (gBotDelayTimer >= 0.5f) // Trễ 0.5 giây mới để AI đánh
        {
            // Gọi logic của Bot.
            // LƯU Ý: Vì bạn nói BotMakeMove() đã tự đánh xuống, tự đổi turn(CELL_X), nên
            // bạn chỉ cần gọi trực tiếp nó ở đây.
            BotMakeMove();

            // Cập nhật trạng thái thắng/thua sau nước đi của Bot
            UpdateGameStateAfterMove();

            if (current().result == RESULT_ONGOING) {
                current().turn = (current().turn == CELL_X) ? CELL_O : CELL_X;
            }

            // Đặt lại số đếm thời gian cho lượt đánh sau
            gBotDelayTimer = 0.0f;
        }
    }
    else
    {
        // [NHÁNH 2: LƯỢT CỦA NGƯỜI CHƠI - BLOCK MOUSE KHỎI BOT]
        // Vì ta đặt trong cụm `else`, nên nếu là isBotTurn == true, click chuột sẽ toàn hoàn bị vô hiệu hóa!
        if (mouse.leftPressed)
        {
            int row = -1, col = -1;
            if (GetCellFromMouse(mouse.position, row, col) && current().board[row][col] == CELL_EMPTY)
            {
                current().board[row][col] = current().turn;
                current().lastMoveRow = row;
                current().lastMoveCol = col;

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

void DrawPlayUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings)
{
    DrawBackgroundOnly();

    if (current().result == RESULT_X_WINS)
        DrawCenteredText(fontSmall, "WINNER: X  |  R: RESET  |  ESC: PAUSE", 70.0f, 24.0f, Color{ 120, 220, 255, 255 });
    else if (current().result == RESULT_O_WINS)
        DrawCenteredText(fontSmall, "WINNER: O  |  R: RESET  |  ESC: PAUSE", 70.0f, 24.0f, Color{ 255, 150, 200, 255 });
    else if (current().result == RESULT_DRAW)
        DrawCenteredText(fontSmall, "DRAW  |  R: RESET  |  ESC: PAUSE", 70.0f, 24.0f, Color{ 240, 225, 225, 220 });
    else if (current().turn == CELL_X)
        DrawCenteredText(fontSmall, "TURN: X  |  5 IN A ROW TO WIN  |  R: RESET  |  ESC: PAUSE", 70.0f, 24.0f, Color{ 120, 220, 255, 255 });
    else
        DrawCenteredText(fontSmall, "TURN: O  |  5 IN A ROW TO WIN  |  R: RESET  |  ESC: PAUSE", 70.0f, 24.0f, Color{ 255, 150, 200, 255 });

    DrawBoardGrid(settings);
    DrawHighlightCell(settings);
    DrawWinLine(settings);
    DrawPieces(fontTitle, settings);

    if (gPaused)
        DrawPauseOverlay(fontTitle, fontSmall, mouse);
}
