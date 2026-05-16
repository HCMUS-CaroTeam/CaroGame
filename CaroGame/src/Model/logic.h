#ifndef LOGIC_H
#define LOGIC_H

/**
 * @file Logic.h
 * @brief Khai báo các hàm xử lý logic của trò chơi Caro
 */

 /**
  * @brief Cập nhật thời gian đếm ngược mỗi frame (Chỉ áp dụng cho Mode Hardcore).
  * Nếu hết giờ, tự động đổi lượt và reset lại bộ đếm thời gian.
  * @return Trả về -99 để báo tín hiệu hết giờ (phục vụ phát âm thanh/UI), ngược lại trả về 0.
  */
int UpdateTimer();

/**
 * @brief Xử lý đặt quân cờ lên bàn cờ dựa trên tọa độ click chuột.
 * Chuyển tọa độ pixel thành tọa độ mảng 2 chiều, kiểm tra tính hợp lệ và cập nhật mảng _BOARD.
 * Tự động reset thời gian đếm ngược (nếu đang ở Mode 2).
 * @param pX Tọa độ pixel X của chuột.
 * @param pY Tọa độ pixel Y của chuột.
 * @return Trả về cờ vừa đánh (-1 hoặc 1). Trả về 0 nếu nằm ngoài bàn cờ hoặc ô đã có sẵn cờ.
 */
int CheckBoard(int pX, int pY);

/**
 * @brief Quét kiểm tra có người chiến thắng hay không dựa trên vị trí quân cờ vừa được đánh.
 * Thuật toán quét theo 4 hướng: Ngang, Dọc, Chéo chính, Chéo phụ.
 * Áp dụng điều kiện thắng tiêu chuẩn (đủ 5 con) hoặc điều kiện Hardcore (đủ 5 con và không chặn 2 đầu).
 * @param lastRow Vị trí dòng (row) của nước đi cuối cùng.
 * @param lastCol Vị trí cột (col) của nước đi cuối cùng.
 * @return Quân cờ chiến thắng (-1 hoặc 1). Trả về 0 nếu chưa thỏa mãn điều kiện thắng.
 */
int TestBoard(int lastRow, int lastCol);

/*
 * @brief RESET ván game.
 */

extern void ResetBoard();

/*
 * @brief Init New Game.
 */

extern void InitNewGame();

/*
* @brief Kiểm tra xem bàn cờ đã đầy chưa (dùng để xét kết quả hòa).
*/

extern bool IsBoardFull();
#endif // LOGIC_H