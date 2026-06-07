#include "Logic.h"

#include "../raylib/include/raylib.h"
#include "Model/game_data.h"

static float TIME_LIMIT = 0.0f;
static float TIME_LEFT = 0.0f;
static bool timerInitialized = false;

int IsValidMove(const int board[][BOARD_SIZE], int row, int col) {
  if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE)
    return 0;
  return board[row][col] == CELL_EMPTY;
}

GameMoveResult ApplyMove(DataGame *game, GameMove move) {
  GameMoveResult result = {0, CELL_EMPTY};
  if (game == nullptr || !IsValidMove(game->board, move.row, move.col))
    return result;
  if (move.piece != CELL_X && move.piece != CELL_O)
    return result;

  game->board[move.row][move.col] = move.piece;
  game->lastMoveRow = move.row;
  game->lastMoveCol = move.col;
  result.accepted = 1;
  result.placedPiece = move.piece;
  return result;
}

GameWinInfo CheckWinOnBoard(const int board[][BOARD_SIZE], int lastRow,
                            int lastCol, int applyBlockRule) {
  GameWinInfo info = {};
  info.result = RESULT_ONGOING;

  if (lastRow < 0 || lastRow >= BOARD_SIZE || lastCol < 0 ||
      lastCol >= BOARD_SIZE)
    return info;

  const int player = board[lastRow][lastCol];
  if (player != CELL_X && player != CELL_O)
    return info;

  const int deltaRows[] = {0, 1, 1, 1};
  const int deltaCols[] = {1, 0, 1, -1};

  for (int direction = 0; direction < 4; direction++) {
    int count = 1;
    int blockedEnds = 0;

    for (int step = 1;; step++) {
      const int row = lastRow + step * deltaRows[direction];
      const int col = lastCol + step * deltaCols[direction];
      if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        blockedEnds++;
        break;
      }
      if (board[row][col] == player) {
        count++;
      } else {
        if (board[row][col] != CELL_EMPTY)
          blockedEnds++;
        break;
      }
    }

    for (int step = 1;; step++) {
      const int row = lastRow - step * deltaRows[direction];
      const int col = lastCol - step * deltaCols[direction];
      if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        blockedEnds++;
        break;
      }
      if (board[row][col] == player) {
        count++;
      } else {
        if (board[row][col] != CELL_EMPTY)
          blockedEnds++;
        break;
      }
    }

    if (count != WIN_LENGTH ||
        (applyBlockRule && blockedEnds >= 2))
      continue;

    int startStep = 0;
    while (true) {
      const int row =
          lastRow + (startStep - 1) * deltaRows[direction];
      const int col =
          lastCol + (startStep - 1) * deltaCols[direction];
      if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE ||
          board[row][col] != player)
        break;
      startStep--;
    }

    for (int index = 0; index < WIN_LENGTH; index++) {
      info.line[index][0] =
          lastRow + (startStep + index) * deltaRows[direction];
      info.line[index][1] =
          lastCol + (startStep + index) * deltaCols[direction];
    }
    info.lineCount = WIN_LENGTH;
    info.result = player == CELL_X ? RESULT_X_WINS : RESULT_O_WINS;
    return info;
  }

  return info;
}

int UpdateTimer() {
  if (current().gameMode == MODE_PVP && current().pvpMode != TOURNAMENT)
    return 0;
  if (current().timeLeft == TIME_LIMIT_NONE)
    return 0;

  if (!timerInitialized) {
    timerInitialized = true;
    if (current().timeLeft == TIME_LIMIT_5S)
      TIME_LIMIT = 5.0f;
    else if (current().timeLeft == TIME_LIMIT_10S)
      TIME_LIMIT = 10.0f;
    else
      TIME_LIMIT = 15.0f;
    TIME_LEFT = TIME_LIMIT;
  }

  if (TIME_LIMIT <= 0.0f)
    return 0;

  TIME_LEFT -= GetFrameTime();
  if (TIME_LEFT > 0.0f)
    return 0;

  current().turn = current().turn == CELL_X ? CELL_O : CELL_X;
  TIME_LEFT = TIME_LIMIT;
  return -99;
}

int CheckBoard(int pixelX, int pixelY) {
  if (pixelX < BOARD_START_X || pixelY < BOARD_START_Y)
    return 0;

  const int col = static_cast<int>((pixelX - BOARD_START_X) / CELL_SIZE);
  const int row = static_cast<int>((pixelY - BOARD_START_Y) / CELL_SIZE);
  GameMove move = {row, col, current().turn};
  GameMoveResult result = ApplyMove(&current(), move);
  if (!result.accepted)
    return 0;

  ResetTurnTimer();
  return result.placedPiece;
}

void ResetTurnTimer() {
  if (current().gameMode == MODE_PVE || current().pvpMode == TOURNAMENT)
    TIME_LEFT = TIME_LIMIT;
}

int TestBoard(int lastRow, int lastCol) {
  const int applyBlockRule =
      current().pvpMode == TOURNAMENT || current().gameMode == MODE_PVE;
  GameWinInfo info =
      CheckWinOnBoard(current().board, lastRow, lastCol, applyBlockRule);
  if (info.result == RESULT_ONGOING)
    return RESULT_ONGOING;

  current().winLineCount = info.lineCount;
  for (int index = 0; index < info.lineCount; index++) {
    current().winLine[index][0] = info.line[index][0];
    current().winLine[index][1] = info.line[index][1];
  }

  if (info.result == RESULT_X_WINS)
    current().scorePlayer1++;
  else if (info.result == RESULT_O_WINS)
    current().scorePlayer2++;
  return info.result;
}

void ResetBoard() {
  for (int row = 0; row < BOARD_SIZE; row++)
    for (int col = 0; col < BOARD_SIZE; col++)
      current().board[row][col] = CELL_EMPTY;

  current().turn = CELL_X;
  current().result = RESULT_ONGOING;
  current().lastMoveRow = -1;
  current().lastMoveCol = -1;
  current().winLineCount = 0;
  for (int index = 0; index < MAX_WIN_LINE; index++) {
    current().winLine[index][0] = -1;
    current().winLine[index][1] = -1;
  }

  timerInitialized = false;
  TIME_LEFT = 0.0f;
}

void InitNewGame() {
  ResetBoard();
  current().nameGame[0] = '\0';
  current().namePlayer1[0] = '\0';
  current().scorePlayer1 = 0;
  current().namePlayer2[0] = '\0';
  current().scorePlayer2 = 0;
  current().gameMode = MODE_PVP;
  current().botDifficulty = DIFFICULTY_NONE;
  current().pvpMode = NONE;
  current().timeLeft = TIME_LIMIT_NONE;
  TIME_LIMIT = 0.0f;
}

bool IsBoardFull() {
  for (int row = 0; row < BOARD_SIZE; row++)
    for (int col = 0; col < BOARD_SIZE; col++)
      if (current().board[row][col] == CELL_EMPTY)
        return false;
  return true;
}

float GetTimeLeft() {
  if (current().gameMode == MODE_PVP && current().pvpMode != TOURNAMENT)
    return -1.0f;
  if (current().timeLeft == TIME_LIMIT_NONE)
    return -1.0f;
  return TIME_LEFT;
}
