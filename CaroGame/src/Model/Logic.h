#ifndef LOGIC_H
#define LOGIC_H

// --------------------------------------------------------
// KHAI BÁO CÁC HÀM XỬ LÝ LOGIC CỦA TRÒ CHƠI
// --------------------------------------------------------

// 1. Quản lý thời gian đếm ngược (Chỉ áp dụng cho Mode 2 - Hardcore)
// Trả về -99 nếu hết giờ, trả về 0 nếu bình thường
int UpdateTimer();

// 2. Kiểm tra nước đi khi người dùng click chuột
// Nhận vào: Tọa độ pixel X, Y của chuột
// Trả về: 0 (nếu đánh không hợp lệ), -1 (Quân X), 1 (Quân O)
int CheckBoard(int pX, int pY);

// 3. Quét radar kiểm tra thắng thua (Có xét luật chặn 2 đầu theo Mode)
// Nhận vào: Chỉ số dòng (row) và cột (col) của nước cờ vừa đánh
// Trả về: 0 (Chưa ai thắng), -1 (X thắng), 1 (O thắng)
int TestBoard(int lastRow, int lastCol);

#endif // LOGIC_H