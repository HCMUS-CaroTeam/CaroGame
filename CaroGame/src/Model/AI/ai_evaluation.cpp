#include "ai_evaluation.h"
#include "Model/game_data.h"

PatternType ClassifyLine(int count, int blocks, int gaps) {
  // LUẬT VIỆT NAM: Đủ 5 con nhưng bị chặn 2 đầu => Vô giá trị
  if (blocks >= 2)
    return PATTERN_NONE;

  // FIX LỖI: NẾU CÓ KHOẢNG TRỐNG (GAP) THÌ TUYỆT ĐỐI KHÔNG ĐƯỢC TÍNH LÀ WIN!
  if (gaps > 0) {
    if (count >= 5)
      count =
          4; // Giáng cấp: 5 quân đứt khúc chỉ tương đương 1 chuỗi 4 cực mạnh
  } else {
    if (count >= 5)
      return WIN; // Không có gap -> Win xịn -> Thắng luôn
  }

  if (blocks == 0) {
    switch (count) {
    case 4:
      return OPEN_4;
    case 3:
      return OPEN_3;
    case 2:
      return OPEN_2;
    case 1:
      return OPEN_1;
    default:
      return PATTERN_NONE;
    }
  } else { // blocks == 1
    switch (count) {
    case 4:
      return BLOCKED_4;
    case 3:
      return BLOCKED_3;
    case 2:
      return BLOCKED_2;
    case 1:
      return BLOCKED_1;
    default:
      return PATTERN_NONE;
    }
  }
}

// ========================================================================
// HÀM CHO BOT EASY (Quét đơn giản + bảng điểm phẳng)
// ========================================================================
long long EvaluatePositionEasy(int r, int c, int piece) {
  long long totalScore = 0;
  int dx[] = {1, 0, 1, 1};
  int dy[] = {0, 1, 1, -1};

  for (int dir = 0; dir < 4; dir++) {
    int count = 1;
    int blocks = 0;

    // Quét TỚI
    for (int step = 1; step <= 4; step++) {
      int nr = r + step * dx[dir];
      int nc = c + step * dy[dir];
      if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
        blocks++;
        break;
      }
      if (current().board[nr][nc] == piece)
        count++;
      else if (current().board[nr][nc] != 0) {
        blocks++;
        break;
      } else
        break;
    }

    // Quét LÙI
    for (int step = 1; step <= 4; step++) {
      int nr = r - step * dx[dir];
      int nc = c - step * dy[dir];
      if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
        blocks++;
        break;
      }
      if (current().board[nr][nc] == piece)
        count++;
      else if (current().board[nr][nc] != 0) {
        blocks++;
        break;
      } else
        break;
    }

    // Phân loại rồi tra bảng điểm Easy
    PatternType type = ClassifyLine(count, blocks, 0);
    if (type != PATTERN_NONE)
      totalScore += EASY_SCORES[type];
  }
  return totalScore;
}

// ========================================================================
// HÀM CHO BOT MEDIUM (Khôi phục nguyên gốc siêu nhẹ của bạn)
// ========================================================================
long long EvaluatePositionMedium(int r, int c, int piece) {
  long long totalScore = 0;
  int dx[] = {1, 0, 1, 1};
  int dy[] = {0, 1, 1, -1};

  for (int dir = 0; dir < 4; dir++) {
    int count = 1;
    int blocks = 0;

    // Quét TỚI
    for (int step = 1; step <= 4; step++) {
      int nr = r + step * dx[dir];
      int nc = c + step * dy[dir];
      if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
        blocks++;
        break;
      }
      if (current().board[nr][nc] == piece)
        count++;
      else if (current().board[nr][nc] != 0) {
        blocks++;
        break;
      } else
        break;
    }

    // Quét LÙI
    for (int step = 1; step <= 4; step++) {
      int nr = r - step * dx[dir];
      int nc = c - step * dy[dir];
      if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
        blocks++;
        break;
      }
      if (current().board[nr][nc] == piece)
        count++;
      else if (current().board[nr][nc] != 0) {
        blocks++;
        break;
      } else
        break;
    }

    // Áp dụng bảng điểm Medium (Học theo hàm CalculateLineScore cũ của bạn)
    if (blocks == 2 && count < 5)
      continue;
    if (count >= 5)
      totalScore += MEDIUM_SCORES[WIN];
    else if (count == 4)
      totalScore +=
          (blocks == 0) ? MEDIUM_SCORES[OPEN_4] : MEDIUM_SCORES[BLOCKED_4];
    else if (count == 3)
      totalScore +=
          (blocks == 0) ? MEDIUM_SCORES[OPEN_3] : MEDIUM_SCORES[BLOCKED_3];
    else if (count == 2)
      totalScore +=
          (blocks == 0) ? MEDIUM_SCORES[OPEN_2] : MEDIUM_SCORES[BLOCKED_2];
    else if (count == 1)
      totalScore +=
          (blocks == 0) ? MEDIUM_SCORES[OPEN_1] : MEDIUM_SCORES[BLOCKED_1];
  }
  return totalScore;
}

long long EvaluatePositionHard(int r, int c, int piece,
                               const int (*board)[BOARD_SIZE],
                               bool isBoardEval) {
  long long totalScore = 0;
  int dx[] = {1, 0, 1, 1};
  int dy[] = {0, 1, 1, -1};

  for (int dir = 0; dir < 4; dir++) {
    // ==========================================
    // VÁ LỖI B: CHỐNG LẠM PHÁT ĐIỂM
    // ==========================================
    if (isBoardEval) {
      int prev_r = r - dx[dir];
      int prev_c = c - dy[dir];
      if (prev_r >= 0 && prev_r < BOARD_SIZE && prev_c >= 0 &&
          prev_c < BOARD_SIZE) {
        // Nếu ô ngay phía sau là cờ của mình -> Chuỗi này đã được đếm từ đầu
        // mút -> Bỏ qua!
        if (board[prev_r][prev_c] == piece)
          continue;

        // Mở rộng: Kiểm tra lùi 2 ô đề phòng chuỗi bị gãy 1 ô (Vá triệt để lạm
        // phát cờ gãy)
        int prev2_r = r - 2 * dx[dir];
        int prev2_c = c - 2 * dy[dir];
        if (board[prev_r][prev_c] == 0 && prev2_r >= 0 &&
            prev2_r < BOARD_SIZE && prev2_c >= 0 && prev2_c < BOARD_SIZE &&
            board[prev2_r][prev2_c] == piece) {
          continue;
        }
      }
    }

    int count = 1;
    int blocks = 0;
    int gaps = 0; // Biến kiểm soát cờ gãy
    int totalGaps =
        0; // Biến mới: Theo dõi tổng số ô trống của cả hướng TỚI và LÙI

    // ==========================================
    // VÁ LỖI A: QUÉT TỚI (Cho phép 1 khoảng trống)
    // ==========================================
    int step = 1;
    while (true) {
      int nr = r + step * dx[dir];
      int nc = c + step * dy[dir];

      if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
        blocks++; // Đụng tường
        break;
      }

      if (board[nr][nc] == piece) {
        count++;
        step++;
      } else if (board[nr][nc] == 0) {
        // FIX LỖI: Nhìn trước 1 ô xem có phải cờ gãy thật không
        int next_r = r + (step + 1) * dx[dir];
        int next_c = c + (step + 1) * dy[dir];

        if (gaps == 0 && next_r >= 0 && next_r < BOARD_SIZE && next_c >= 0 &&
            next_c < BOARD_SIZE &&
            board[next_r][next_c] == piece) { // Đằng sau ô trống LÀ CỜ CỦA MÌNH
          gaps++;      // Châm trước cho qua 1 ô trống đầu tiên
          totalGaps++; // Đếm tổng số ô trống
          step++;
        } else {
          break; // Gặp ô trống thứ 2 -> Đứt chuỗi thật sự
        }
      } else {
        blocks++; // Đụng địch
        break;
      }
    }

    // ==========================================
    // QUÉT LÙI
    // ==========================================
    step = 1;
    gaps = 0; // Reset gap để quét công bằng về phía sau
    while (true) {
      int nr = r - step * dx[dir];
      int nc = c - step * dy[dir];

      if (nr < 0 || nr >= BOARD_SIZE || nc < 0 || nc >= BOARD_SIZE) {
        blocks++;
        break;
      }

      if (board[nr][nc] == piece) {
        count++;
        step++;
      } else if (board[nr][nc] == 0) {
        // FIX LỖI (Chú ý dấu trừ cho chiều lùi)
        int next_r = r - (step + 1) * dx[dir];
        int next_c = c - (step + 1) * dy[dir];

        if (gaps == 0 && next_r >= 0 && next_r < BOARD_SIZE && next_c >= 0 &&
            next_c < BOARD_SIZE && board[next_r][next_c] == piece) {
          gaps++;
          totalGaps++;
          step++;
        } else {
          break;
        }
      } else {
        blocks++;
        break;
      }
    }

    PatternType type = ClassifyLine(count, blocks, totalGaps);
    if (type != PATTERN_NONE)
      totalScore += HARD_SCORES[type];
  }
  return totalScore;
}
