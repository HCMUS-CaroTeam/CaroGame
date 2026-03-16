#include "../../include/Model.h"
#include <fstream>

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

void SaveGameProgress(const char *filename)
{
    // Tạo một đối tượng Progress để lưu trữ thông tin hiện tại của game
    Progress progress;
    // Điền thông tin người chơi
    cout << "Nhap ten tran dau: ";
    int length = 0;
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

void LoadGameProgress(const char *filename, vector<Progress> &history)
{
    ifstream inFile(filename, ios::binary);
    if (!inFile)
    {
        cerr << "Khong the mo file de doc tru!" << endl;
        return;
    }

    Progress progress;
    while (inFile.read(reinterpret_cast<char *>(&progress), sizeof(Progress)))
    {
        history.push_back(progress);
    }
    inFile.close();
}

void PrintGameHistory()
{
    //vector<Progress> history;
    //LoadGameProgress("game_progress.dat", history);
    //cout << "Lich su tran dau:" << endl;
    //for (const auto &progress : history)
    //{
    //    cout << "Ten tran dau: " << progress.gameName << endl;
    //    cout << "Nguoi choi X: " << progress.playerXName << " - Diem: " << progress.playerXScore << endl;
    //    cout << "Nguoi choi O: " << progress.playerOName << " - Diem: " << progress.playerOScore << endl;
    //    cout << "So van da choi: " << progress.numberOfRounds << endl;
    //    cout << "Luot choi hien tai: " << (progress.isXTurn ? "X" : "O") << endl;
    //    cout << "-----------------------------" << endl;
    //}
}

void GarbageCollect()
{
    // Dọn dẹp tài nguyên nếu có khai báo con trỏ
}