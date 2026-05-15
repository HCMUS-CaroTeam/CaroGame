//#pragma once
//
//// Đưa toàn bộ bảng điểm ra mặt tiền để ai cũng thấy
//// QUAN TRỌNG: Open-3 phải >> Blocked-4 vì Open-3 trở thành Open-4 (bất khả
//// chiến bại)
////             Blocked-4 chỉ có 1 đầu mở, dễ phòng thủ hơn nhiều.
//constexpr long long SCORE_WIN = 1000000000; // 1 tỷ       — Thắng chắc
//constexpr long long SCORE_OPEN4 =
//    100000000; // 100 triệu  — Gần như thắng (2 đầu mở, 4 quân)
//constexpr long long SCORE_OPEN3 =
//    10000000; // 10 triệu   — Đe dọa chết người (sẽ thành Open-4)
//constexpr long long SCORE_BLOCKED4 =
//    1000000; // 1 triệu    — Nguy hiểm nhưng chặn được (1 đầu bịt)
//constexpr long long SCORE_OPEN2 = 100000; // 100 ngàn   — Tiềm năng tấn công tốt
//constexpr long long SCORE_BLOCKED3 = 10000; // 10 ngàn    — Mối đe dọa nhỏ
//constexpr long long SCORE_BLOCKED2 = 100;
//constexpr long long SCORE_OPEN1 = 10;
//constexpr long long SCORE_BLOCKED1 = 1;
//
//long long CalculateLineScore(int count, int blocks, int gaps); 
//long long EvaluatePosition(int r, int c, int piece, bool isBoardEval = false);


enum PatternType {
    WIN, OPEN_4, BLOCKED_4, OPEN_3, BLOCKED_3, OPEN_2, BLOCKED_2, OPEN_1, BLOCKED_1, PATTERN_NONE
};

// BẢNG ĐIỂM DÀNH CHO MINIMAX (Bot Hard)
constexpr long long HARD_SCORES[] = {
    1000000000LL, // WIN
    100000000LL,   // OPEN_4
    10000000LL,    // OPEN_3
    1000000LL,     // BLOCKED_4
	100000LL,      // OPEN_2
	10000LL,       // BLOCKED_3
    100LL,        // BLOCKED_2
    10LL,         // OPEN_1
    1LL           // BLOCKED_1
};

// BẢNG ĐIỂM DÀNH CHO HEURISTIC (Bot Medium)
constexpr long long MEDIUM_SCORES[] = {
    1000000000LL, // WIN
    10000000LL,   // OPEN_4
    5000000LL,    // BLOCKED_4 
    10000LL,      // OPEN_3
    1000LL,       // BLOCKED_3
    100LL,        // OPEN_2
    10LL,         // BLOCKED_2
    2LL,          // OPEN_1
    1LL           // BLOCKED_1
};

// BẢNG ĐIỂM DÀNH CHO BOT EASY (Chỉ nhận ra mối nguy trước mắt, còn lại gần như mù)
constexpr long long EASY_SCORES[] = {
    1000000000LL, // WIN
    10000000LL,   // OPEN_4
    5000000LL,    // BLOCKED_4
    10LL,         // OPEN_3     — Gần như không thấy
    5LL,          // BLOCKED_3  — Gần như không thấy
    1LL,          // OPEN_2
    0LL,          // BLOCKED_2
    0LL,          // OPEN_1
    0LL           // BLOCKED_1
};

PatternType ClassifyLine(int count, int blocks, int gaps);

// Tách riêng hàm đánh giá điểm cho từng level:
long long EvaluatePositionEasy(int r, int c, int piece);
long long EvaluatePositionMedium(int r, int c, int piece);
long long EvaluatePositionHard(int r, int c, int piece, bool isBoardEval = false);