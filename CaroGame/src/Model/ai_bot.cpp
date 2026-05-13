#include "ai_bot.h"
#include "ai_easy.h"
#include "ai_medium.h"
#include "ai_hard.h"
#include "Model/game_data.h"
#include "raylib.h"


// ========================================================================
// HÀM GỌI TỪ GAME LOOP (Hàm duy nhất giao tiếp với bên ngoài)
// ========================================================================
void BotMakeMove() {
    int r = -1, c = -1;

    // Phân luồng độ khó 
    if (current().botDifficulty == DIFFICULTY_EASY) {
        GetEasyMove(r, c);
    }
    else if (current().botDifficulty == DIFFICULTY_MEDIUM) {
        GetMediumMove(r, c); 
    }
    else if (current().botDifficulty == DIFFICULTY_HARD) {
        GetHardMove(r, c);
    }
    // Thực hiện nước đi
    if (r != -1 && c != -1 && current().board[r][c] == 0) {
        current().board[r][c] = current().turn;
        current().lastMoveRow = r;
        current().lastMoveCol = c;
    }
}