#include "Scenes/Play/ui_play.h"
#include "Scenes/Setting/ui_setting.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/config.h"
#include "Control/menu_data.h"
#include <cmath>

static constexpr int BOARD_SIZE = 8;
static constexpr int CELL_SIZE  = 72;
static constexpr int WIN_LENGTH = 5;

static constexpr float BOARD_PIXEL_SIZE = BOARD_SIZE * CELL_SIZE;
static constexpr float BOARD_START_X    = SCREEN_WIDTH  * 0.5f - BOARD_PIXEL_SIZE * 0.5f;
static constexpr float BOARD_START_Y    = 220.0f;

enum CellValue
{
    CELL_EMPTY = 0,
    CELL_X,
    CELL_O
};

static int  gBoard[BOARD_SIZE][BOARD_SIZE] = {};
static int  gCurrentPlayer = CELL_X;
static int  gWinner        = CELL_EMPTY;
static bool gDraw          = false;
static bool gPaused        = false;
static const char* gPauseMessage = "";

// Last placed piece (for highlight last move)
static int gLastRow = -1;
static int gLastCol = -1;

// Winning cells (filled when gWinner != CELL_EMPTY)
static int gWinCells[WIN_LENGTH][2] = {};

// ─── Board helpers ────────────────────────────────────────────────────
static Rectangle GetBoardRect()
{
    return { BOARD_START_X, BOARD_START_Y, BOARD_PIXEL_SIZE, BOARD_PIXEL_SIZE };
}

static void ResetBoard()
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
            gBoard[r][c] = CELL_EMPTY;

    gCurrentPlayer = CELL_X;
    gWinner        = CELL_EMPTY;
    gDraw          = false;
    gPaused        = false;
    gPauseMessage  = "";
    gLastRow       = -1;
    gLastCol       = -1;
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

static bool IsBoardFull()
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
            if (gBoard[r][c] == CELL_EMPTY) return false;
    return true;
}

// Checks 5-in-a-row in one direction; if found and outCells != nullptr, stores positions
static bool CheckDirectionWin(int startRow, int startCol, int dRow, int dCol, int player, int (*outCells)[2] = nullptr)
{
    for (int step = 0; step < WIN_LENGTH; ++step)
    {
        int r = startRow + dRow * step;
        int c = startCol + dCol * step;
        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE) return false;
        if (gBoard[r][c] != player) return false;
    }
    if (outCells)
        for (int step = 0; step < WIN_LENGTH; ++step)
        {
            outCells[step][0] = startRow + dRow * step;
            outCells[step][1] = startCol + dCol * step;
        }
    return true;
}

static bool CheckFiveInRow(int player)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (gBoard[r][c] != player) continue;
            if (CheckDirectionWin(r, c, 0,  1, player, gWinCells)) return true;
            if (CheckDirectionWin(r, c, 1,  0, player, gWinCells)) return true;
            if (CheckDirectionWin(r, c, 1,  1, player, gWinCells)) return true;
            if (CheckDirectionWin(r, c, 1, -1, player, gWinCells)) return true;
        }
    return false;
}

static void UpdateGameStateAfterMove()
{
    if (CheckFiveInRow(CELL_X)) { gWinner = CELL_X; return; }
    if (CheckFiveInRow(CELL_O)) { gWinner = CELL_O; return; }
    if (IsBoardFull())           { gDraw = true; }
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
    if (!settings.highlightLastMove || gLastRow < 0) return;

    float x = BOARD_START_X + gLastCol * CELL_SIZE + 1.0f;
    float y = BOARD_START_Y + gLastRow * CELL_SIZE + 1.0f;
    float s = (float)CELL_SIZE - 2.0f;

    DrawRectangle((int)x, (int)y, (int)s, (int)s, Color{ 255, 245, 160, 35 });
    DrawRectangleLinesEx({ x, y, s, s }, 2.0f, Color{ 255, 230, 80, 140 });
}

static void DrawWinLine(const AppSettings& settings)
{
    if (gWinner == CELL_EMPTY) return;

    Color col = (gWinner == CELL_X)
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
    float x1 = BOARD_START_X + gWinCells[0][1]           * CELL_SIZE + CELL_SIZE * 0.5f;
    float y1 = BOARD_START_Y + gWinCells[0][0]           * CELL_SIZE + CELL_SIZE * 0.5f;
    float x2 = BOARD_START_X + gWinCells[WIN_LENGTH-1][1] * CELL_SIZE + CELL_SIZE * 0.5f;
    float y2 = BOARD_START_Y + gWinCells[WIN_LENGTH-1][0] * CELL_SIZE + CELL_SIZE * 0.5f;
    DrawLineEx({ x1, y1 }, { x2, y2 }, 5.0f, col);
}

static void DrawPieces(Font fontTitle, const AppSettings& settings)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (gBoard[r][c] == CELL_EMPTY) continue;

            const char* text  = (gBoard[r][c] == CELL_X) ? "X" : "O";
            Color        color = (gBoard[r][c] == CELL_X)
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
                cellRect.x + cellRect.width  * 0.5f - size.x * 0.5f,
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
                gPauseMessage = "SAVE NOT IMPLEMENTED YET";
                break;
            case PAUSE_BTN_LOAD:
                gPauseMessage = "LOAD NOT IMPLEMENTED YET";
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
        SCREEN_WIDTH  * 0.5f - 240.0f,
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
        bool pressed  = hovered && mouse.leftDown;
        DrawUIButton(animIndex, gPauseButtons[i], fontSmall, hovered, pressed);
    }

    if (gPauseMessage && gPauseMessage[0] != '\0')
    {
        DrawCenteredText(fontSmall, gPauseMessage,
            panel.y + panel.height - 44.0f, 20.0f, Color{ 255, 210, 210, 220 });
    }
}

// ─── Public API ───────────────────────────────────────────────────────
void InitPlayUI()  { ResetBoard(); }
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
    if (IsKeyPressed(KEY_R))  { ResetBoard(); return; }

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

    if (gWinner != CELL_EMPTY || gDraw) return;

    if (mouse.leftPressed)
    {
        int row = -1, col = -1;
        if (GetCellFromMouse(mouse.position, row, col) && gBoard[row][col] == CELL_EMPTY)
        {
            gBoard[row][col] = gCurrentPlayer;
            gLastRow = row;
            gLastCol = col;

            UpdateGameStateAfterMove();

            if (gWinner == CELL_EMPTY && !gDraw)
                gCurrentPlayer = (gCurrentPlayer == CELL_X) ? CELL_O : CELL_X;
        }
    }
}

void DrawPlayUI(Font fontTitle, Font fontSmall, const MouseState& mouse, const AppSettings& settings)
{
    DrawBackgroundOnly();

    DrawCenteredText(fontTitle, "PLAY", 130.0f, 42.0f, Color{ 255, 235, 225, 255 });

    if (gWinner == CELL_X)
        DrawCenteredText(fontSmall, "WINNER: X  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 120, 220, 255, 255 });
    else if (gWinner == CELL_O)
        DrawCenteredText(fontSmall, "WINNER: O  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 255, 150, 200, 255 });
    else if (gDraw)
        DrawCenteredText(fontSmall, "DRAW  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 240, 225, 225, 220 });
    else if (gCurrentPlayer == CELL_X)
        DrawCenteredText(fontSmall, "TURN: X  |  5 IN A ROW TO WIN  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 120, 220, 255, 255 });
    else
        DrawCenteredText(fontSmall, "TURN: O  |  5 IN A ROW TO WIN  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 255, 150, 200, 255 });

    DrawBoardGrid(settings);
    DrawHighlightCell(settings);
    DrawWinLine(settings);
    DrawPieces(fontTitle, settings);

    if (gPaused)
        DrawPauseOverlay(fontTitle, fontSmall, mouse);
}
