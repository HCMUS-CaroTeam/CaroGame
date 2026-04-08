#include "Scenes/Play/ui_play.h"
#include "View/ui_background.h"
#include "Model/config.h"
#include "Scenes/Play/ui_pause.h"

static constexpr int BOARD_SIZE = 8;
static constexpr int CELL_SIZE = 72;
static constexpr int WIN_LENGTH = 5;

static constexpr float BOARD_PIXEL_SIZE = BOARD_SIZE * CELL_SIZE;
static constexpr float BOARD_START_X = SCREEN_WIDTH * 0.5f - BOARD_PIXEL_SIZE * 0.5f;
static constexpr float BOARD_START_Y = 220.0f;

enum CellValue
{
    CELL_EMPTY = 0,
    CELL_X,
    CELL_O
};

static int gBoard[BOARD_SIZE][BOARD_SIZE] = {};
static int gCurrentPlayer = CELL_X;
static int gWinner = CELL_EMPTY;
static bool gDraw = false;

static Rectangle GetBoardRect()
{
    return Rectangle{
        BOARD_START_X,
        BOARD_START_Y,
        BOARD_PIXEL_SIZE,
        BOARD_PIXEL_SIZE
    };
}

static void ResetBoard()
{
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            gBoard[r][c] = CELL_EMPTY;
        }
    }

    gCurrentPlayer = CELL_X;
    gWinner = CELL_EMPTY;
    gDraw = false;
    ClosePause();
}

static bool IsInsideBoard(Vector2 pos)
{
    Rectangle boardRect = GetBoardRect();

    return (
        pos.x >= boardRect.x &&
        pos.x <= boardRect.x + boardRect.width &&
        pos.y >= boardRect.y &&
        pos.y <= boardRect.y + boardRect.height
        );
}

static bool GetCellFromMouse(Vector2 pos, int& outRow, int& outCol)
{
    if (!IsInsideBoard(pos))
    {
        return false;
    }

    outCol = static_cast<int>((pos.x - BOARD_START_X) / CELL_SIZE);
    outRow = static_cast<int>((pos.y - BOARD_START_Y) / CELL_SIZE);

    if (outRow < 0 || outRow >= BOARD_SIZE || outCol < 0 || outCol >= BOARD_SIZE)
    {
        return false;
    }

    return true;
}

static bool IsBoardFull()
{
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (gBoard[r][c] == CELL_EMPTY)
            {
                return false;
            }
        }
    }

    return true;
}

static bool CheckDirectionWin(int startRow, int startCol, int dRow, int dCol, int player)
{
    for (int step = 0; step < WIN_LENGTH; ++step)
    {
        int r = startRow + dRow * step;
        int c = startCol + dCol * step;

        if (r < 0 || r >= BOARD_SIZE || c < 0 || c >= BOARD_SIZE)
        {
            return false;
        }

        if (gBoard[r][c] != player)
        {
            return false;
        }
    }

    return true;
}

static bool CheckFiveInRow(int player)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (gBoard[r][c] != player)
            {
                continue;
            }

            if (CheckDirectionWin(r, c, 0, 1, player))  return true;
            if (CheckDirectionWin(r, c, 1, 0, player))  return true;
            if (CheckDirectionWin(r, c, 1, 1, player))  return true;
            if (CheckDirectionWin(r, c, 1, -1, player)) return true;
        }
    }

    return false;
}

static void UpdateGameStateAfterMove()
{
    if (CheckFiveInRow(CELL_X))
    {
        gWinner = CELL_X;
        return;
    }

    if (CheckFiveInRow(CELL_O))
    {
        gWinner = CELL_O;
        return;
    }

    if (IsBoardFull())
    {
        gDraw = true;
    }
}

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

static void DrawBoardGrid()
{
    Rectangle boardRect = GetBoardRect();

    DrawRectangleRounded(boardRect, 0.02f, 8, Color{ 18, 18, 30, 150 });
    DrawRectangleLinesEx(boardRect, 3.0f, Color{ 220, 230, 255, 220 });

    for (int i = 1; i < BOARD_SIZE; ++i)
    {
        float x = BOARD_START_X + i * CELL_SIZE;
        float y = BOARD_START_Y + i * CELL_SIZE;

        DrawLineEx(
            Vector2{ x, BOARD_START_Y },
            Vector2{ x, BOARD_START_Y + BOARD_PIXEL_SIZE },
            2.0f,
            Color{ 180, 200, 255, 190 }
        );

        DrawLineEx(
            Vector2{ BOARD_START_X, y },
            Vector2{ BOARD_START_X + BOARD_PIXEL_SIZE, y },
            2.0f,
            Color{ 180, 200, 255, 190 }
        );
    }
}

static void DrawPieces(Font fontTitle)
{
    for (int r = 0; r < BOARD_SIZE; ++r)
    {
        for (int c = 0; c < BOARD_SIZE; ++c)
        {
            if (gBoard[r][c] == CELL_EMPTY)
            {
                continue;
            }

            const char* text = (gBoard[r][c] == CELL_X) ? "X" : "O";
            Color color = (gBoard[r][c] == CELL_X)
                ? Color{ 120, 220, 255, 255 }
            : Color{ 255, 150, 200, 255 };

            Rectangle cellRect{
                BOARD_START_X + c * CELL_SIZE,
                BOARD_START_Y + r * CELL_SIZE,
                static_cast<float>(CELL_SIZE),
                static_cast<float>(CELL_SIZE)
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

void InitPlayUI()
{
    ResetBoard();
    InitPauseUI();
}

void ShutdownPlayUI()
{
    ShutdownPauseUI();
}

void UpdatePlayUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen,
    bool& shouldClose
)
{
    if (IsKeyPressed(KEY_R))
    {
        ResetBoard();
        return;
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        TogglePause();
        return;
    }

    if (IsPauseActive())
    {
        UpdatePauseUI(mouse, dt, audio, settings, currentScreen, shouldClose);
        return;
    }

    if (gWinner != CELL_EMPTY || gDraw)
    {
        return;
    }

    if (mouse.leftPressed)
    {
        int row = -1;
        int col = -1;

        if (GetCellFromMouse(mouse.position, row, col))
        {
            if (gBoard[row][col] == CELL_EMPTY)
            {
                gBoard[row][col] = gCurrentPlayer;

                UpdateGameStateAfterMove();

                if (gWinner == CELL_EMPTY && !gDraw)
                {
                    gCurrentPlayer = (gCurrentPlayer == CELL_X) ? CELL_O : CELL_X;
                }
            }
        }
    }
}

void DrawPlayUI(Font fontTitle, Font fontSmall, const MouseState& mouse)
{
    DrawBackgroundOnly();

    DrawCenteredText(fontTitle, "PLAY", 130.0f, 42.0f, Color{ 255, 235, 225, 255 });

    if (gWinner == CELL_X)
    {
        DrawCenteredText(fontSmall, "WINNER: X  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 120, 220, 255, 255 });
    }
    else if (gWinner == CELL_O)
    {
        DrawCenteredText(fontSmall, "WINNER: O  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 255, 150, 200, 255 });
    }
    else if (gDraw)
    {
        DrawCenteredText(fontSmall, "DRAW  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 240, 225, 225, 220 });
    }
    else if (gCurrentPlayer == CELL_X)
    {
        DrawCenteredText(fontSmall, "TURN: X  |  5 IN A ROW TO WIN  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 120, 220, 255, 255 });
    }
    else
    {
        DrawCenteredText(fontSmall, "TURN: O  |  5 IN A ROW TO WIN  |  R: RESET  |  ESC: PAUSE", 180.0f, 24.0f, Color{ 255, 150, 200, 255 });
    }

    DrawBoardGrid();
    DrawPieces(fontTitle);

    if (IsPauseActive())
    {
        DrawPauseUI(fontTitle, fontSmall, mouse);
    }
}