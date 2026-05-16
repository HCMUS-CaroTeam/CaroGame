#include "ai_hard.h"
#include "Model/game_data.h"
#include "ai_evaluation.h"
#include <algorithm> // Để xài hàm std::max, std::min
#include <cstring>   // Để xài hàm memcpy

constexpr int MAX_DEPTH = 5; // Dùng constexpr thay cho #define cho an toàn hơn
const long long INF = 10000000000000000LL;

// Định nghĩa kiểu bàn cờ "ảo" (scratch board) để tránh thay đổi bàn cờ thật
typedef int ScratchBoard[BOARD_SIZE][BOARD_SIZE];

struct Move {
    int r, c;
    long long score;
}; // Thêm điểm số vào Struct ảo để Sort

// ========================================================================
// HÀM HỖ TRỢ: SAO CHÉP BÀN CỜ THẬT SANG BÀN CỜ ẢO
// ========================================================================
static void CopyBoard(const int src[BOARD_SIZE][BOARD_SIZE], ScratchBoard dst) {
    memcpy(dst, src, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
}

// ========================================================================
// HÀM HỖ TRỢ: GHI BÀN CỜ ẢO NGƯỢC LẠI BÀN CỜ THẬT (phục hồi an toàn)
// ========================================================================
static void RestoreBoard(int dst[BOARD_SIZE][BOARD_SIZE], const ScratchBoard src) {
    memcpy(dst, src, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
}

// ========================================================================
// HÀM HỖ TRỢ: TẠM THỜI ĐỒNG BỘ BÀN CỜ ẢO VÀO current().board
// Vì EvaluatePosition đọc trực tiếp từ current().board, ta cần đồng bộ
// bàn cờ ảo vào current().board trước khi gọi hàm đánh giá.
// ========================================================================
// static void SyncToLiveBoard(const ScratchBoard scratch) {
//    memcpy(current().board, scratch, sizeof(int) * BOARD_SIZE * BOARD_SIZE);
//}

static int GetBeamWidth(int depth, int totalMoves) {
  // Giai đoạn đầu game (ít quân): quét rộng hơn để không bỏ nước khai cuộc
  // Càng sâu trong Minimax: cắt càng mạnh để tiết kiệm thời gian
  if (totalMoves < 10)
    return std::min(totalMoves, 25); // Đầu game: rộng
  if (depth >= 4)
    return std::min(
        totalMoves,
        15); // Sâu nhất: vừa phải (cũ: 10 → quá hẹp, bỏ sót nước thủ)
  if (depth >= 2)
    return std::min(totalMoves, 20); // Giữa: thoáng (cũ: 15)
  return std::min(totalMoves, 25);   // Nông: rộng nhất
}

static int GetSearchRadius(const ScratchBoard scratch) {
  // Đếm số quân đang có trên bàn
  int pieceCount = 0;
  for (int r = 0; r < BOARD_SIZE; r++)
    for (int c = 0; c < BOARD_SIZE; c++)
      if (scratch[r][c] != 0)
        pieceCount++;

  if (pieceCount <= 6)
    return 3; // Khai cuộc: quét rộng
  if (pieceCount <= 20)
    return 2; // Trung cuộc: vừa
  return 1;   // Tàn cuộc: hẹp lại cho nhanh
}

// ========================================================================
// HÀM HỖ TRỢ 1: TÌM CÁC NƯỚC ĐI TIỀM NĂNG (Khoanh vùng tìm kiếm)
// Giờ đọc từ scratch board thay vì current().board
// ========================================================================
static int GeneratePotentialMoves(Move moves[], int botPiece, int playerPiece,
                                  ScratchBoard scratch, int depth) {
  int radius = GetSearchRadius(scratch); // ← tính một lần trước vòng lặp
  int count = 0;
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      if (scratch[r][c] == 0) {
        // Kiểm tra neighbor siêu tốc (chỉ cần 1 ô bên cạnh có cờ là đủ, ko cần
        // quét bán kính 2)
        bool hasNeighbor = false;
        for (int dr = -radius; dr <= radius && !hasNeighbor;
             dr++) { // ← radius động
          for (int dc = -radius; dc <= radius && !hasNeighbor; dc++) {
            int nr = r + dr, nc = c + dc;
            if (nr >= 0 && nr < BOARD_SIZE && nc >= 0 && nc < BOARD_SIZE &&
                scratch[nr][nc] != 0) {
              hasNeighbor = true;
            }
          }
        }

        if (hasNeighbor) {
          // Chấm điểm tức thời để ưu tiên
          long long atk = EvaluatePositionHard(r, c, botPiece, scratch);
          long long def = EvaluatePositionHard(r, c, playerPiece, scratch);
          moves[count++] = {r, c, atk + def};
        }
      }
    }
  }

  // Sort các nước đi để Alpha-Beta Pruning làm việc hiệu suất tối đa (Move
  // Ordering)
  std::sort(moves, moves + count,
            [](const Move &a, const Move &b) { return a.score > b.score; });

  // Trong GeneratePotentialMoves, thay dòng cuối:
  // return std::min(count, 15);  ← Xóa dòng này
  return GetBeamWidth(depth, count); // ← Truyền depth vào hàm
}

// ========================================================================
// HÀM HỖ TRỢ 2: ĐÁNH GIÁ TOÀN CỤC BÀN CỜ
// Giờ đọc từ scratch board thay vì current().board
// ========================================================================
static long long EvaluateBoard(int botPiece, int playerPiece,
                               ScratchBoard scratch) {

  long long totalScore = 0;
  for (int r = 0; r < BOARD_SIZE; r++) {
    for (int c = 0; c < BOARD_SIZE; c++) {
      if (scratch[r][c] == botPiece) {
        long long score = EvaluatePositionHard(r, c, botPiece, scratch, true);
        // Nếu BOT xếp được 5 quân -> Thắng chắc -> Dừng hàm!
        if (score >= HARD_SCORES[WIN])
          return INF / 2;
        totalScore += score;
      } else if (scratch[r][c] == playerPiece) {
        long long score =
            EvaluatePositionHard(r, c, playerPiece, scratch, true);
        // Nếu NGƯỜI CHƠI xếp được 5 quân -> Thua chắc -> Dừng hàm!
        if (score >= HARD_SCORES[WIN])
          return -INF / 2;
        totalScore -=
            score * 3 / 2; // Dùng số nguyên thay vì 1.5 để tránh truncation
      }
    }
  }
  return totalScore;
}

// ========================================================================
// THUẬT TOÁN MINIMAX KẾT HỢP ALPHA-BETA PRUNING
// Giờ thao tác trên scratch board thay vì current().board
// ========================================================================
static long long Minimax(int depth, long long alpha, long long beta,
                         bool isMaximizing, int botPiece, int playerPiece,
                         ScratchBoard scratch) {
  // CHỈ đánh giá toàn cục khi chạm đáy hoặc hết bàn cờ
  if (depth == 0)
    return EvaluateBoard(botPiece, playerPiece, scratch);

  Move potentialMoves[BOARD_SIZE * BOARD_SIZE];
  int moveCount = GeneratePotentialMoves(potentialMoves, botPiece, playerPiece,
                                         scratch, depth);

  if (moveCount == 0)
    return EvaluateBoard(botPiece, playerPiece, scratch);

  if (isMaximizing) {
    long long maxEval = -INF;
    for (int i = 0; i < moveCount; i++) {
      int r = potentialMoves[i].r;
      int c = potentialMoves[i].c;

      scratch[r][c] = botPiece; // Đánh thử trên bàn cờ ẢO

      long long eval = 0;
      // KIỂM TRA DỪNG SỚM SIÊU TỐC: Chỉ chấm điểm đúng ô vừa đánh
      // Cần đồng bộ trước khi gọi EvaluatePosition
      if (EvaluatePositionHard(r, c, botPiece, scratch) >= HARD_SCORES[WIN]) {
        eval = INF / 2 + depth; // Ưu tiên thắng nhanh
      } else {
        eval = Minimax(depth - 1, alpha, beta, false, botPiece, playerPiece,
                       scratch);
      }

      scratch[r][c] = 0; // Backtracking trên bàn cờ ẢO

      maxEval = std::max(maxEval, eval);
      alpha = std::max(alpha, eval);
      if (beta <= alpha)
        break;
    }
    return maxEval;
  } else {
    long long minEval = INF;
    for (int i = 0; i < moveCount; i++) {
      int r = potentialMoves[i].r;
      int c = potentialMoves[i].c;

      scratch[r][c] = playerPiece; // Người đánh thử trên bàn cờ ẢO

      long long eval = 0;
      // Kịch bản xấu: Người chơi đi nước này là mình thua luôn
      if (EvaluatePositionHard(r, c, playerPiece, scratch) >=
          HARD_SCORES[WIN]) {
        eval = -INF / 2 - depth; // Ưu tiên thua chậm (phòng thủ)
      } else {
        eval = Minimax(depth - 1, alpha, beta, true, botPiece, playerPiece,
                       scratch);
      }

      scratch[r][c] = 0; // Backtracking trên bàn cờ ẢO

      minEval = std::min(minEval, eval);
      beta = std::min(beta, eval);
      if (beta <= alpha)
        break;
    }
    return minEval;
  }
}

// ========================================================================
// HÀM CHÍNH CHO TRÙM CUỐI
// ========================================================================
void GetHardMove(int &outRow, int &outCol) {
  int botPiece = current().turn;
  int playerPiece = (botPiece == CELL_X) ? CELL_O : CELL_X;

  // === AN TOÀN: Tạo bản sao toàn bộ bàn cờ thật ===
  ScratchBoard backup;
  CopyBoard(current().board, backup);

  // === Tạo scratch board cho toàn bộ quá trình tìm kiếm ===
  ScratchBoard scratch;
  CopyBoard(current().board, scratch);

  Move potentialMoves[BOARD_SIZE * BOARD_SIZE];
  int moveCount = GeneratePotentialMoves(potentialMoves, botPiece, playerPiece,
                                         scratch, MAX_DEPTH);

  if (moveCount == 0) {
    outRow = BOARD_SIZE / 2;
    outCol = BOARD_SIZE / 2;
    // Phục hồi bàn cờ thật trước khi return
    RestoreBoard(current().board, backup);
    return;
  }

  long long bestScore = -INF;
  int bestRow = -1, bestCol = -1;

  for (int i = 0; i < moveCount; i++) {
    int r = potentialMoves[i].r;
    int c = potentialMoves[i].c;

    scratch[r][c] = botPiece; // Đánh thử trên bàn cờ ẢO

    long long score = 0;
    // ƯU TIÊN #1: Kiểm tra xem bước này có thắng luôn không
    if (EvaluatePositionHard(r, c, botPiece, scratch) >= HARD_SCORES[WIN]) {
      outRow = r;
      outCol = c;
      // Phục hồi bàn cờ thật trước khi return
      RestoreBoard(current().board, backup);
      return; // Trúng Jackpot, trả kết quả về luôn!
    }

    scratch[r][c] = 0; // Backtracking trên bàn cờ ẢO (cho lượt check tấn công)
  }

  // ƯU TIÊN #2: Kiểm tra phòng thủ ngay lập tức — nếu đối thủ đánh vào ô nào đó
  // là THẮNG LUÔN → chặn ngay!
  for (int i = 0; i < moveCount; i++) {
    int r = potentialMoves[i].r;
    int c = potentialMoves[i].c;

    scratch[r][c] = playerPiece; // Giả lập đối thủ đánh vào đây
    if (EvaluatePositionHard(r, c, playerPiece, scratch) >= HARD_SCORES[WIN]) {
      outRow = r;
      outCol = c;
      RestoreBoard(current().board, backup);
      return; // Phải chặn ngay, không thì thua!
    }
    scratch[r][c] = 0; // Backtracking
  }

  // ƯU TIÊN #3: Nếu không có thắng/thua ngay → chạy Minimax để tìm nước tốt
  // nhất
  for (int i = 0; i < moveCount; i++) {
    int r = potentialMoves[i].r;
    int c = potentialMoves[i].c;

    scratch[r][c] = botPiece;
    long long score = Minimax(MAX_DEPTH - 1, -INF, INF, false, botPiece,
                              playerPiece, scratch);
    scratch[r][c] = 0;

    if (score > bestScore) {
      bestScore = score;
      bestRow = r;
      bestCol = c;
    }
  }

  // === PHỤC HỒI BÀN CỜ THẬT (đảm bảo không bao giờ bị hỏng) ===
  RestoreBoard(current().board, backup);

  outRow = bestRow;
  outCol = bestCol;
}