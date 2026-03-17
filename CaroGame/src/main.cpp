#define WIN32_LEAN_AND_MEAN
#define NOGDI              
#define NOUSER

#include "raylib.h"
#include <vector>
#include "../include/View.h"
#include "../include/Model.h"
#include "../include/Control.h"

int gameState = 0;
int menuSelected = 0;

// Cấu hình kích thước để có chỗ cho bảng điểm bên phải
const int screenWidth = 1000; // Tăng chiều rộng để chứa DrawPlayerStats()
const int screenHeight = 700;
float cellSize;
float offsetX = 50.0f;
float offsetY = 80.0f;

int main() {
    InitWindow(screenWidth, screenHeight, "Caro Game - Stats Integrated");
    SetTargetFPS(60);

    // Kích thước bàn cờ (Giả sử bàn cờ chiếm khoảng 600px chiều rộng)
    cellSize = 35.0f;

    while (!WindowShouldClose()) {
        // --- LOGIC (Giữ nguyên như bản trước) ---
        if (gameState == 0) {
            if (IsKeyPressed(KEY_UP)) menuSelected = (menuSelected - 1 + 4) % 4;
            if (IsKeyPressed(KEY_DOWN)) menuSelected = (menuSelected + 1) % 4;
            if (IsKeyPressed(KEY_ENTER)) {
                if (menuSelected == 0) gameState = 1;
                else if (menuSelected == 1) gameState = 3;
                else if (menuSelected == 2) { StartGame(true); gameState = 1; }
                else if (menuSelected == 3) break;
            }
        }
        else if (gameState == 1) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Vector2 mousePos = GetMousePosition();
                if (mousePos.x >= offsetX && mousePos.x <= offsetX + BOARD_SIZE * cellSize &&
                    mousePos.y >= offsetY && mousePos.y <= offsetY + BOARD_SIZE * cellSize) {

                    _COL = (int)((mousePos.x - offsetX) / cellSize);
                    _ROW = (int)((mousePos.y - offsetY) / cellSize);

                    if (CheckBoard() != 0) {
                        int result = TestBoard();
                        if (ProcessFinish(result) != 2)
                        {
                            gameState = 2;
                        }
                    }
                }
            }

            if (IsKeyPressed(KEY_SPACE)) {
                StartGame(true);
                gameState = 0;
            }

            if (IsKeyPressed(KEY_S)) {
                SaveGameProgress("game_progress.dat");
            }
            if (IsKeyPressed(KEY_SPACE)) {
                StartGame(true);
                gameState = 0;
            }

        }
        else if (gameState == 2) {
            if (IsKeyPressed(KEY_S)) {
                SaveGameProgress("game_progress.dat");
            }
            if (IsKeyPressed(KEY_ENTER)) { StartGame(); gameState = 1; }
            if (IsKeyPressed(KEY_SPACE)) {
                StartGame(true);
                gameState = 0;
            }
        }

        // --- VẼ GIAO DIỆN ---
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (gameState == 0) {
            DrawMainMenu(menuSelected);
        }
        else if (gameState == 1 || gameState == 2) {
            // 1. Vẽ thông tin hướng dẫn
            DrawText("SPACE: Menu | Chuot trai: Danh co", 50, 20, 20, DARKGRAY);

            // 2. VẼ BẢNG ĐIỂM (Từ File 2)
            // Hàm này thường vẽ ở phía bên phải màn hình dựa trên logic trong View.cpp của bạn
            DrawPlayerStats();

            // 3. Vẽ bàn cờ (Style File 1)
            for (int i = 0; i < BOARD_SIZE; i++) {
                for (int j = 0; j < BOARD_SIZE; j++) {
                    Rectangle cellRect = { offsetX + j * cellSize, offsetY + i * cellSize, cellSize, cellSize };
                    DrawRectangleLinesEx(cellRect, 1.0f, LIGHTGRAY);

                    if (_BOARD[i][j] == -1) {
                        DrawText("X", (int)(cellRect.x + cellSize / 4), (int)(cellRect.y + cellSize / 8), (int)(cellSize * 0.8), RED);
                    }
                    else if (_BOARD[i][j] == 1) {
                        DrawText("O", (int)(cellRect.x + cellSize / 4), (int)(cellRect.y + cellSize / 8), (int)(cellSize * 0.8), BLUE);
                    }
                }
            }

            // 4. Thông báo kết thúc (Nếu có)
            if (gameState == 2) {

                int answer = AskContinue(TestBoard());
                if (answer == 1) {
                    StartGame();
                    gameState = 1;
                }
                else if (answer == -1) {
                    gameState = 0;
                    ResetData(true);
                }
            }
        }
        else if (gameState == 3) {
            std::vector<Progress> history = LoadGameProgress("game_progress.dat");
            DrawHistoryMenu(history, 0);
            if (IsKeyPressed(KEY_SPACE)) {
                StartGame(true);
                gameState = 0;
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}