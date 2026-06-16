# CaroGame

Game Caro (Gomoku) 2D viết bằng C++ và Raylib, hỗ trợ chơi PvP, PvE với 3 cấp độ AI, lưu/tải game, và nhiều chế độ chơi.

---

## Mục lục

- [Tổng quan](#tổng-quan)
- [Tính năng](#tính-năng)
- [Yêu cầu hệ thống](#yêu-cầu-hệ-thống)
- [Build & Chạy](#build--chạy)
- [Cấu trúc project](#cấu-trúc-project)
- [Luật chơi](#luật-chơi)
- [Hệ thống AI](#hệ-thống-ai)
- [Cài đặt & Lưu game](#cài-đặt--lưu-game)
- [Developer Tools](#developer-tools)

---

## Tổng quan

**CaroGame** là game cờ Caro trên bàn 15×15. Người chơi đặt quân X hoặc O lần lượt, người đầu tiên tạo được đúng 5 quân liên tiếp theo hàng ngang, dọc hoặc chéo thì thắng.

| Thông số | Giá trị |
|---|---|
| Ngôn ngữ | C++20 |
| Đồ họa | Raylib |
| Build | Visual Studio 2022+ (MSBuild) |
| Nền tảng | Windows |
| Độ phân giải | 1600 × 900 (60 FPS) |

---

## Tính năng

### Chế độ chơi

| Chế độ | Mô tả |
|---|---|
| **PvP Classic** | 2 người chơi, thắng khi đủ đúng 5 quân liên tiếp |
| **PvP Tournament** | 2 người chơi, áp thêm luật Block (5 quân bị chặn 2 đầu không tính thắng) |
| **PvE Easy** | Người vs AI dễ |
| **PvE Medium** | Người vs AI trung bình |
| **PvE Hard** | Người vs AI khó |

### Giới hạn thời gian mỗi lượt

Không giới hạn / 5 giây / 10 giây / 15 giây.

### Giao diện & Tiện ích

- Chọn tên và avatar cho từng người chơi (4 nhân vật + boss)
- Highlight nước đi cuối cùng
- Hiển thị tọa độ bàn cờ
- Điều chỉnh âm lượng nhạc nền và SFX riêng biệt
- Điều chỉnh độ sáng màn hình
- Màn hình Story / Giới thiệu
- Màn hình Credits (About)

### Lưu / Tải game

- Lưu nhiều ván với tên tùy chỉnh
- Tải lại từ danh sách file save
- Đổi tên và xóa save

---

## Yêu cầu hệ thống

- **OS:** Windows 10/11
- **IDE:** Visual Studio 2022 hoặc mới hơn (toolset v145)
- **Thư viện:** Raylib (đã đi kèm trong `CaroGame/raylib/`, không cần cài thêm)

---

## Build & Chạy

1. Mở file `CaroGame.sln` bằng Visual Studio 2022+.
2. Chọn cấu hình build: `Debug` hoặc `Release`, nền tảng `x64` hoặc `x86`.
3. Nhấn **Build → Build Solution** (`Ctrl+Shift+B`).
4. Chạy bằng **Debug → Start Debugging** (`F5`) hoặc chạy trực tiếp file `.exe` trong thư mục output.

> **Lưu ý:** Chạy từ thư mục chứa thư mục `CaroGame/Assets/` để game load được tài nguyên.

### Chế độ dòng lệnh (Developer)

```
CaroGame.exe --ai-tests           # Chạy test suite logic AI
CaroGame.exe --heuristic-demo     # In heatmap đánh giá heuristic
CaroGame.exe --ai-vs-ai [N]       # AI tự đấu N ván (mặc định 6)
```

---

## Cấu trúc project

```
CaroGame/
├── CaroGame.sln
└── CaroGame/
    ├── settings.cfg              # Cài đặt người dùng (tự động lưu)
    ├── raylib/                   # Thư viện Raylib (bundled)
    │   ├── include/
    │   └── lib/
    ├── Assets/
    │   ├── audio/
    │   │   ├── BackGround/       # Nhạc nền Regular & Battle
    │   │   └── SFX/              # Âm thanh click, hover, đặt quân, thắng/thua
    │   ├── avatars/              # Ảnh nhân vật (4 + boss)
    │   ├── bg/                   # Ảnh nền các màn hình
    │   ├── buttons/              # Texture nút bấm
    │   ├── fonts/                # Born2bSportyFS.otf (hỗ trợ tiếng Việt)
    │   ├── frame/                # Viền/khung UI
    │   ├── Modes/                # Icon chế độ chơi
    │   ├── texture/              # Texture màn hình Settings
    │   ├── WIN-LOSE EFFECT/      # Animation thắng/thua
    │   └── X-O/                  # Sprite quân X và O
    └── src/
        ├── main.cpp
        ├── Model/
        │   ├── config.h          # Hằng số màn hình, bàn cờ, FPS
        │   ├── game_data.h/cpp   # DataGame — trạng thái ván game
        │   ├── app_settings.h    # AppSettings — cài đặt + lưu/tải settings.cfg
        │   ├── logic.h/cpp       # Luật chơi: đặt quân, kiểm tra thắng, timer
        │   └── AI/
        │       ├── ai_bot.h/cpp          # Dispatcher phân phối theo độ khó
        │       ├── ai_easy.h/cpp         # AI Easy
        │       ├── ai_medium.h/cpp       # AI Medium
        │       ├── ai_hard.h/cpp         # AI Hard
        │       ├── ai_evaluation.h/cpp   # Đánh giá pattern bàn cờ
        │       └── ai_technical_demo.h/cpp # Benchmark & testing
        ├── Control/
        │   ├── input_mouse.h/cpp      # Trạng thái chuột, hover/click
        │   ├── menu_data.h/cpp        # Định nghĩa button toàn bộ scene
        │   └── name_input_data.h/cpp  # Button cho màn hình nhập tên
        ├── View/
        │   ├── ui_background.h/cpp    # Render background
        │   ├── ui_button.h/cpp        # Render & animation button
        │   └── ui_frame.h/cpp         # Render khung/panel UI
        ├── Systems/
        │   ├── audio_manager.h/cpp    # Quản lý audio (nhạc nền + SFX)
        │   └── save_load.h/cpp        # Lưu/tải game (binary serialization)
        └── Scenes/
            ├── MainMenu/             # Màn hình chính
            ├── Setup/                # Chọn chế độ chơi, độ khó, thời gian
            ├── Story/                # Màn hình câu chuyện/intro
            ├── Play/                 # Gameplay chính + pause menu
            ├── Setting/              # Cài đặt âm thanh, độ sáng
            ├── About/                # Thông tin & credits
            ├── Save_Load/            # Giao diện lưu/tải game
            └── Notify/               # Dialog xác nhận (thoát, về menu)
```

### Kiến trúc MVC

- **Model** (`Model/`): Dữ liệu game và logic thuần túy, không phụ thuộc UI.
- **View** (`View/` + `Scenes/`): Render và hiển thị.
- **Control** (`Control/`): Xử lý input, điều hướng giữa các màn hình qua `ScreenState`.

---

## Luật chơi

- Bàn cờ **15×15**.
- Hai người (hoặc người và Bot) lần lượt đặt quân X và O.
- **Thắng:** Tạo được đúng **5** quân liên tiếp theo hàng ngang, dọc hoặc chéo.
- **Luật Overline** (áp dụng mọi chế độ): Chuỗi **hơn 5** quân liên tiếp **không tính thắng**.
- **Luật Block** (Tournament và PvE): 5 quân bị **chặn cả 2 đầu** cũng **không tính thắng**.
- **Hòa:** Bàn cờ đầy mà không ai thắng.

---

## Hệ thống AI

### Easy

- Heuristic đơn giản dựa trên bảng điểm pattern.
- Thêm **25% ngẫu nhiên** để AI không quá máy móc.
- Phù hợp cho người mới chơi.

### Medium

- Thuật toán **Minimax** với độ sâu tìm kiếm **2**.
- Lọc ô ứng viên từ vùng lân cận quân đã đặt (giảm không gian tìm kiếm).
- Cân bằng giữa tốc độ và chiến thuật.

### Hard

- Thuật toán **Minimax** với độ sâu **5** + **Alpha-Beta Pruning**.
- Beam width giới hạn số nước xem xét ở mỗi tầng.
- Theo dõi thống kê tìm kiếm: số node duyệt, số nhánh cắt, thời gian tính toán.
- Nhận biết các pattern: WIN, OPEN_4, BLOCKED_4, OPEN_3, v.v.

### Bảng điểm pattern (ước lượng)

| Pattern | Điểm |
|---|---|
| WIN (5 liên tiếp) | 1,000,000,000 |
| OPEN_4 (4 + 2 đầu trống) | 100,000,000 |
| BLOCKED_4 (4 + 1 đầu trống) | 1,000,000 |
| OPEN_3 | 10,000 |
| Các pattern thấp hơn | < 1,000 |

---

## Cài đặt & Lưu game

### File settings.cfg

Lưu tự động khi thoát, load tự động khi khởi động. Các tham số:

```
backgroundGrid=1        # Hiện lưới bàn cờ (0/1)
uiBrightness=0.7        # Độ sáng UI (0.0 - 1.0)
musicVolume=0.6         # Âm lượng nhạc nền (0.0 - 1.0)
sfxVolume=0.75          # Âm lượng SFX (0.0 - 1.0)
highlightLastMove=1     # Highlight nước đi cuối (0/1)
showCoordinates=0       # Hiện tọa độ bàn cờ (0/1)
gameMode=0              # 0=PvP, 1=PvE
botDifficulty=0         # 0=Easy, 1=Medium, 2=Hard
pvpMode=0               # 0=Classic, 1=Tournament
timeLeft=0              # 0=Vô hạn, 1=5s, 2=10s, 3=15s
boardSize=15
```

### Lưu game

- Lưu nhiều ván với tên tùy chỉnh.
- Dữ liệu serialize nhị phân (`#pragma pack(1)`) để đảm bảo tính toàn vẹn.
- Hỗ trợ đổi tên và xóa save từ giao diện.

---

## Developer Tools

Chạy qua command line để test AI mà không cần mở giao diện:

```bash
# Chạy toàn bộ test suite AI logic
CaroGame.exe --ai-tests

# In heatmap heuristic trực quan ra console
CaroGame.exe --heuristic-demo

# AI tự đấu nhau N ván và in kết quả thống kê
CaroGame.exe --ai-vs-ai 10
```

---

## Nhóm phát triển

| Thành viên | Đóng góp chính |
|---|---|
| Thien | Save/load logic |
| Loi | Play scene redesign |
| Tri | AI benchmark, build system |
| Linh | UI, scene design |
