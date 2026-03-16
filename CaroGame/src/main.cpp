#define WIN32_LEAN_AND_MEAN
#define NOGDI              
#define NOUSER
#include "raylib.h"
#include "../include/View.h"
#include "../include/Model.h"
#include "../include/Control.h"

// Trạng thái game: 0 là Menu, 1 là đang chơi
int gameState = 0;
int menuSelected = 0; // Lựa chọn trong menu

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Caro Game - Loi_UI Project");
    SetTargetFPS(60);

    StartGame();

    while (!WindowShouldClose())
    {
        if (gameState == 0) // Đang ở màn hình chính
        {
            if (IsKeyPressed(KEY_UP)) menuSelected = (menuSelected - 1 + 3) % 3;
            if (IsKeyPressed(KEY_DOWN)) menuSelected = (menuSelected + 1) % 3;

            if (IsKeyPressed(KEY_ENTER)) {
                if (menuSelected == 0) gameState = 1; 
            }
        }
        else if (gameState == 1)
        {
            if (IsKeyPressed(KEY_A) || IsKeyPressed(KEY_LEFT))  MoveLeft();
            if (IsKeyPressed(KEY_D) || IsKeyPressed(KEY_RIGHT)) MoveRight();
            if (IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP))    MoveUp();
            if (IsKeyPressed(KEY_S) || IsKeyPressed(KEY_DOWN))  MoveDown();

            if (IsKeyPressed(KEY_ENTER))
            {
                if (CheckBoard() != 0)
                {
                    int result = TestBoard();
                    if (ProcessFinish(result) != 2)
                    {
                        StartGame();
                    }
                }
            }
            if (IsKeyPressed(KEY_ESCAPE)) gameState = 0;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (gameState == 0)
        {
            DrawMainMenu(menuSelected); 
        }
        else
        {
            DrawBoard(15);      // Vẽ lưới & tọa độ
            DrawPieces();       // Vẽ X và O từ mảng _BOARD
            DrawPlayerStats();  // Vẽ bảng điểm bên phải

            // Vẽ con trỏ tại vị trí hiện tại
            DrawRectangleLinesEx({ (float)_X, (float)_Y, 30, 30 }, 3, GREEN);

            DrawText("Di chuyen: WASD | Danh: ENTER | Menu: ESC", 10, 10, 20, DARKGRAY);
        }
        EndDrawing();
    }

    CloseWindow();
    return 0;
}