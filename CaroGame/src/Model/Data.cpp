#include "../../include/Model.h"
#include <fstream>
#include <raylib.h>
using namespace std;

// ĐỊNH NGHĨA BIẾN TOÀN CỤC TẠI ĐÂY (Chỉ 1 lần duy nhất)
char _BOARD[BOARD_SIZE][BOARD_SIZE]; // Mảng này dùng để lưu trạng thái bàn cờ khi cần thiết (ví dụ: lưu vào file)
int _ROW, _COL;                      // Biến toàn cục lưu vị trí logic của ô hiện hành (hàng, cột)
bool _TURN;                          // Biến toàn cục lưu lượt chơi hiện hành (true = X, false = O)
int _COMMAND;                        // Biến toàn cục lưu lệnh người dùng
int _X, _Y;                          // Biến toàn cục lưu vị trí hiện hành của con trỏ màn hình
short XScore = 0;                    // Điểm số của người chơi X
short OScore = 0;                    // Điểm số của người chơi O
short NumberOfRounds = 1;            // Số ván đã chơi (bắt đầu từ 1)

void ResetData(bool resetGame)
{
    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            // Reset toàn bộ bàn cờ về trạng thái trống (0 hoặc ký tự khoảng trắng)
            _BOARD[i][j] = 0;
        }
    }

    // Reset lượt chơi và lệnh
    _TURN = true; // X đi trước
    _COMMAND = -1;

    // Reset vị trí logic (hàng 0, cột 0)
    _ROW = 0;
    _COL = 0;

    if (resetGame)
    {
        XScore = 0;
        OScore = 0;
        NumberOfRounds = 0;
    }

    // Tính toán tọa độ màn hình thực tế cho con trỏ tại ô (0, 0)
    // Dựa trên công thức cũ của bạn: x = 4*j + LEFT + 2 và y = 2*i + TOP + 1
    _X = 100 + (_COL * 30);
    _Y = 80 + (_ROW * 30);
}
//


void SaveGameProgress(const char* filename)
{
    // Tạo một đối tượng Progress để lưu trữ thông tin hiện tại của game
    Progress progress;
    // Điền thông tin người chơi
    
    strcpy_s(progress.gameName, sizeof(progress.gameName), "SaveGame1");
    strcpy_s(progress.playerXName, sizeof(progress.playerXName), "Player X"); // Có thể thay bằng tên thực tế nếu có
    strcpy_s(progress.playerOName, sizeof(progress.playerOName), "Player O"); // Có thể thay bằng tên thực tế nếu có
    
    // Điền thông tin trận đấu (có thể cập nhật sau mỗi ván)
    progress.playerXScore = XScore;
    progress.playerOScore = OScore;
    progress.numberOfRounds = NumberOfRounds; // Cập nhật số ván đã chơi nếu có
    // Sao chép trạng thái bàn cờ hiện tại vào cấu trúc Progress

    for (int i = 0; i < BOARD_SIZE; i++)
    {
        for (int j = 0; j < BOARD_SIZE; j++)
        {
            progress.boardState[i][j] = _BOARD[i][j];
        }
    }
    progress.isXTurn = _TURN;

    ofstream outFile(filename, ios::binary | ios::app);
    if (!outFile)
    {
        cerr << "Khong the mo file de luu tru!" << endl;
        return;
    }

    // Ghi cấu trúc Progress vào file
    outFile.write(reinterpret_cast<const char *>(&progress), sizeof(Progress));
    outFile.close();
}

vector<Progress> LoadGameProgress(const char* filename)
{
    vector<Progress> history;
    ifstream inFile(filename, ios::binary);
    if (!inFile)
    {
        cerr << "Khong the mo file de doc tru!" << endl;
        return history;
    }

    Progress progress;
    while (inFile.read(reinterpret_cast<char *>(&progress), sizeof(Progress)))
    {
        history.push_back(progress);
    }
    inFile.close();
    return history;
}

void DrawHistoryMenu(const vector<Progress>& history, int selectedIdx) {
    DrawText("--- LICH SU TRAN DAU ---", 250, 50, 30, DARKGRAY);
    DrawText("Bam [ENTER] de xem lai | Bam [ESC] de quay lai", 200, 90, 15, GRAY);

    if (history.empty()) {
        DrawText("Chua co du lieu luu!", 300, 250, 20, LIGHTGRAY);
        return;
    }

    for (int i = 0; i < (int)history.size(); i++) {
        // Tính toán vị trí Y: mỗi dòng cách nhau 40 pixel
        int posY = 150 + i * 45;

        // Tạo hiệu ứng highlight cho dòng đang chọn
        Color textColor = BLACK;
        if (i == selectedIdx) {
            textColor = RED;
            DrawRectangle(140, posY - 5, 520, 35, Fade(LIGHTGRAY, 0.5f)); // Vẽ nền cho dòng đang chọn
            DrawText("> ", 150, posY, 20, RED);
        }

        // Format chuỗi hiển thị: "Tên ván - Player 1 vs Player 2 - 1's score vs 2's score"
        // Giả sử Progress có các trường: saveName, p1Name, p2Name, p1Score, p2Score
        char buffer[256];
        sprintf_s(buffer, "%d. %s: %s (%d) vs %s (%d)", i + 1,
            history[i].gameName, history[i].playerXName, history[i].playerXScore,
            history[i].playerOName, history[i].playerOScore);

        DrawText(buffer, 170, posY, 20, textColor);
    }
}

void GarbageCollect()
{
    // Dọn dẹp tài nguyên nếu có khai báo con trỏ
}