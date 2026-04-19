#include "Scenes/Setting/ui_setting.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Control/input_mouse.h"
#include "Model/config.h"
#include <algorithm>

using namespace std;

//  Texture wrapper
struct SettingTex
{
    Texture2D t{};
    bool loaded    = false;
    bool attempted = false;
};

static SettingTex gTrackDark;   // pixil-layer-4  — inactive track (dark maroon)
static SettingTex gTrackTeal;   // pixil-layer-5  — active track (teal)
static SettingTex gKnob;        // pixil-layer-6  — draggable knob circle
static SettingTex gBtnMusic;    // pixil-layer-7  — music icon label button
static SettingTex gBtnSFX;      // pixil-layer-8  — sfx icon label button
static SettingTex gBtnPlain;    // pixil-layer-9  — plain label button

//  Runtime state
static int         gActiveDrag    = -1;
static float       gToggleAnim[1] = {};
static bool        gNeedsReset    = true;
static bool        gSliderHover[3]= {};
static bool        gToggleHover[1]= {};
static ScreenState gReturnScreen  = SCREEN_MAIN_MENU;  // màn hình sẽ quay về khi Back

//  Layout constants
// ── PANEL (khung nền chính) ───────────────────────────────────────────
// PANEL_X / PANEL_Y : góc trên-trái của panel
// PANEL_W / PANEL_H : chiều rộng / chiều cao panel
// Muốn dịch panel sang trái/phải → chỉnh PANEL_X
// Muốn dịch panel lên/xuống     → chỉnh PANEL_Y
static constexpr float PANEL_X = 400.0f;   // ← dịch trái/phải toàn bộ panel
static constexpr float PANEL_Y = 190.0f;   // ← dịch lên/xuống toàn bộ panel
static constexpr float PANEL_W = 800.0f;   // ← độ rộng panel
static constexpr float PANEL_H = 480.0f;   // ← độ cao panel

// ── KÍCH THƯỚC CONTROL ───────────────────────────────────────────────
// BTN_W / BTN_H  : kích thước nút label bên trái mỗi dòng
// SLD_W          : độ dài thanh slider
// TOG_W          : độ dài thanh toggle (nên giữ nhỏ ~80)
// KNOB_W/KNOB_H  : kích thước nút tròn kéo được
static constexpr float BTN_W   = 210.0f;   // ← độ rộng nút label (đủ rộng cho chữ "SOUND EFFECT")
static constexpr float BTN_H   = 55.0f;    // ← độ cao nút label
static constexpr float TRACK_H = 20.0f;    // ← độ cao thanh track
static constexpr float SLD_W   = 400.0f;   // ← độ dài thanh slider (tăng = dài hơn)
static constexpr float TOG_W   = 80.0f;    // ← độ dài thanh toggle bật/tắt
static constexpr float KNOB_W  = 37.0f;    // ← độ rộng nút tròn kéo
static constexpr float KNOB_H  = 34.0f;    // ← độ cao nút tròn kéo

// ── VỊ TRÍ CỘT (toàn bộ control nằm 1 cột, dùng chung BTN_X & TRACK_X)
// Chỉnh BTN_X   → dịch nút label + thanh track sang trái / phải cùng lúc
// TRACK_X tự tính = BTN_X + BTN_W + 18 (khoảng cách giữa nút và thanh)
// Nếu đổi BTN_W thì phải cập nhật TRACK_X thủ công cho đúng
static constexpr float BTN_X   = 430.0f;   // ← vị trí X nút label (dịch = dịch cả hàng)
static constexpr float TRACK_X = 658.0f;   // ← vị trí X thanh track (= BTN_X + BTN_W + 18)

// ── VỊ TRÍ TỪNG HÀNG SLIDER (tọa độ Y tâm) ──────────────────────────
//   SLD_CENTER_Y[0] → hàng nút [MUSIC]        + thanh kéo âm lượng nhạc
//   SLD_CENTER_Y[1] → hàng nút [SOUND EFFECT] + thanh kéo âm lượng SFX
//   SLD_CENTER_Y[2] → hàng nút [BRIGHTNESS]   + thanh kéo độ sáng
static constexpr float SLD_CENTER_Y[3] = {
    305.0f,   // ← Y hàng MUSIC         (tăng = xuống thấp hơn)
    385.0f,   // ← Y hàng SOUND EFFECT  (tăng = xuống thấp hơn)
    465.0f,   // ← Y hàng BRIGHTNESS    (tăng = xuống thấp hơn)
};
static constexpr float SLD_BTN_X[3]   = { BTN_X, BTN_X, BTN_X };
static constexpr float SLD_TRACK_X[3] = { TRACK_X, TRACK_X, TRACK_X };

// ── VỊ TRÍ TỪNG HÀNG TOGGLE (tọa độ Y tâm) ──────────────────────────
//   TOG_CENTER_Y[0] → hàng nút [HIGHLIGHT] + công tắc bật/tắt highlight nước đi
static constexpr float TOG_CENTER_Y[1] = {
    545.0f,   // ← Y hàng HIGHLIGHT LAST MOVE  (tăng = xuống thấp hơn)
};
static constexpr float TOG_BTN_X[1]   = { BTN_X };
static constexpr float TOG_TRACK_X[1] = { TRACK_X };


//  Texture helpers
static bool TexReady(const Texture2D& t) { return t.id != 0; }

static void LoadSettingTex(SettingTex& st, const char* pathA, const char* pathB = nullptr)
{
    if (st.attempted) return;
    st.attempted = true;

    if (pathA && FileExists(pathA))
        st.t = LoadTexture(pathA);
    else if (pathB && FileExists(pathB))
        st.t = LoadTexture(pathB);

    st.loaded = TexReady(st.t);
    if (!st.loaded)
        TraceLog(LOG_WARNING, "SETTING: Could not load %s", pathA ? pathA : "?");
}

static void UnloadSettingTex(SettingTex& st)
{
    if (st.loaded) { UnloadTexture(st.t); st.t = {}; st.loaded = false; }
    st.attempted = false;
}

static void DrawTexFull(const SettingTex& st, Rectangle dest, Color tint = WHITE)
{
    if (!st.loaded) return;
    DrawTexturePro(
        st.t,
        { 0.0f, 0.0f, (float)st.t.width, (float)st.t.height },
        dest, { 0.0f, 0.0f }, 0.0f, tint
    );
}

// Draw left portion of a texture (clips source by fraction for slider fill)
static void DrawTexClipLeft(const SettingTex& st, Rectangle dest, float fraction)
{
    if (!st.loaded || fraction <= 0.001f) return;
    float srcW = (float)st.t.width * fraction;
    DrawTexturePro(
        st.t,
        { 0.0f, 0.0f, srcW, (float)st.t.height },
        dest, { 0.0f, 0.0f }, 0.0f, WHITE
    );
}

//  Draw helpers
static void DrawCenteredInRange(Font font, const char* text,
    float x1, float x2, float y, float fontSize, Color color)
{
    Vector2 sz = MeasureTextEx(font, text, fontSize, 1.0f);
    float x = x1 + (x2 - x1) * 0.5f - sz.x * 0.5f;
    DrawTextEx(font, text, { x, y }, fontSize, 1.0f, color);
}

static void DrawSectionHeader(Font font, const char* text,
    float x1, float x2, float y)
{
    constexpr Color TEXT_COL = { 215, 195, 210, 230 };
    constexpr Color LINE_COL = { 180, 155, 175, 110 };
    constexpr float FS       = 16.0f;

    DrawCenteredInRange(font, text, x1, x2, y, FS, TEXT_COL);

    float lineY = y + FS + 5.0f;
    DrawLineEx({ x1 + 10.0f, lineY }, { x2 - 10.0f, lineY }, 1.2f, LINE_COL);
}

static void DrawLabelButton(const SettingTex& btnTex,
    const char* label, float btnX, float centerY,
    Font font, bool hovered)
{
    Rectangle r = { btnX, centerY - BTN_H * 0.5f, BTN_W, BTN_H };
    Color tint  = hovered ? Color{ 255, 230, 235, 255 } : WHITE;
    DrawTexFull(btnTex, r, tint);

    if (label && label[0] != '\0')
    {
        float FS = (float)font.baseSize;   // ← tự lấy kích thước native của font được truyền vào
        Vector2 sz  = MeasureTextEx(font, label, FS, 1.0f);
        Vector2 pos = {
            r.x + r.width  * 0.5f - sz.x * 0.5f,
            r.y + r.height * 0.5f - sz.y * 0.5f
        };
        DrawTextEx(font, label, pos, FS, 1.0f, Color{ 255, 242, 248, 255 });
    }
}

static void DrawSliderControl(float trackX, float centerY, float value, bool active)
{
    float ty = centerY - TRACK_H * 0.5f;

    // Full inactive (dark) track
    DrawTexFull(gTrackDark, { trackX, ty, SLD_W, TRACK_H });

    // Teal fill from left up to value
    if (value > 0.001f)
        DrawTexClipLeft(gTrackTeal, { trackX, ty, SLD_W * value, TRACK_H }, value);

    // Knob — brighten slightly when being dragged
    float kx   = trackX + SLD_W * value - KNOB_W * 0.5f;
    float ky   = centerY - KNOB_H * 0.5f;
    Color tint = active ? Color{ 255, 210, 210, 255 } : WHITE;
    DrawTexFull(gKnob, { kx, ky, KNOB_W, KNOB_H }, tint);
}

static void DrawToggleControl(float trackX, float centerY, float anim)
{
    float ty = centerY - TRACK_H * 0.5f;

    // Base inactive track
    DrawTexFull(gTrackDark, { trackX, ty, TOG_W, TRACK_H });

    // Teal overlay fades in as anim approaches 1
    if (anim > 0.001f)
    {
        unsigned char alpha = (unsigned char)(255.0f * anim);
        DrawTexFull(gTrackTeal, { trackX, ty, TOG_W, TRACK_H }, { 255, 255, 255, alpha });
    }

    // Knob slides from left (OFF) to right (ON)
    float kx = trackX + anim * (TOG_W - KNOB_W);
    float ky = centerY - KNOB_H * 0.5f;
    DrawTexFull(gKnob, { kx, ky, KNOB_W, KNOB_H });
}

//  Hit rectangles
static Rectangle SliderHit(float trackX, float centerY)
{
    // Slightly larger than the track so knob edges are easy to grab
    return { trackX - KNOB_W * 0.5f, centerY - 22.0f, SLD_W + KNOB_W, 44.0f };
}

static Rectangle ToggleRowHit(float btnX, float centerY)
{
    return { btnX, centerY - BTN_H * 0.5f, BTN_W + 18.0f + TOG_W + KNOB_W, BTN_H };
}

//  Public API
void InitSettingUI()
{
    LoadSettingTex(gTrackDark, "assets/buttons/pixil-layer-4.png");
    LoadSettingTex(gTrackTeal, "assets/buttons/pixil-layer-5.png");
    LoadSettingTex(gKnob,      "assets/buttons/pixil-layer-6.png");
    LoadSettingTex(gBtnMusic,  "assets/buttons/pixil-layer-7.png");
    LoadSettingTex(gBtnSFX,    "assets/buttons/pixil-layer-8.png");
    LoadSettingTex(gBtnPlain,  "assets/buttons/pixil-layer-9.png");

    gActiveDrag  = -1;
    gNeedsReset  = true;
    for (int i = 0; i < 3; ++i) gSliderHover[i] = false;
    gToggleAnim[0]  = 0.0f;
    gToggleHover[0] = false;
}

void SetSettingReturnScreen(ScreenState from) { gReturnScreen = from; }

void ShutdownSettingUI()
{
    UnloadSettingTex(gTrackDark);
    UnloadSettingTex(gTrackTeal);
    UnloadSettingTex(gKnob);
    UnloadSettingTex(gBtnMusic);
    UnloadSettingTex(gBtnSFX);
    UnloadSettingTex(gBtnPlain);
}

void UpdateSettingUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    AppSettings& settings,
    ScreenState& currentScreen
)
{
    // On first frame after entering this screen: snap toggle anims to actual values
    if (gNeedsReset)
    {
        gToggleAnim[0] = settings.highlightLastMove ? 1.0f : 0.0f;
        gActiveDrag    = -1;
        for (int i = 0; i < 3; ++i) gSliderHover[i] = false;
        gToggleHover[0] = false;
        gNeedsReset    = false;
    }

    // Pointers into AppSettings — order must match SLD_* and TOG_* arrays above
    float* sliders[3] = {
        &settings.musicVolume,
        &settings.sfxVolume,
        &settings.uiBrightness
    };
    bool* toggles[1] = {
        &settings.highlightLastMove
    };

    //  Sliders
    for (int i = 0; i < 3; ++i)
    {
        Rectangle hit = SliderHit(SLD_TRACK_X[i], SLD_CENTER_Y[i]);
        bool over = IsMouseOverRect(mouse, hit);

        if (over && !gSliderHover[i]) PlayMenuHover(audio);
        gSliderHover[i] = over;

        if (over && mouse.leftPressed)
            gActiveDrag = i;

        if (gActiveDrag == i)
        {
            if (mouse.leftDown)
            {
                float t = (mouse.position.x - SLD_TRACK_X[i]) / SLD_W;
                *sliders[i] = max(0.0f, min(1.0f, t));
            }
            if (mouse.leftReleased)
                gActiveDrag = -1;
        }
    }

    //  Toggles
    for (int i = 0; i < 1; ++i)
    {
        Rectangle hit = ToggleRowHit(TOG_BTN_X[i], TOG_CENTER_Y[i]);
        bool over = IsMouseOverRect(mouse, hit);

        if (over && !gToggleHover[i]) PlayMenuHover(audio);
        gToggleHover[i] = over;

        if (over && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);
            *toggles[i] = !(*toggles[i]);
        }

        // Animate knob toward current bool value
        float target = *toggles[i] ? 1.0f : 0.0f;
        float step   = dt * 8.0f;
        if (gToggleAnim[i] < target) gToggleAnim[i] = min(target, gToggleAnim[i] + step);
        else                         gToggleAnim[i] = max(target, gToggleAnim[i] - step);
    }

    //  BACK button (animIndex = 30) 
    {
        bool hovered = false, pressed = false;
        UpdateUIButton(30, gSettingButtons[0], mouse, dt, audio, settings, hovered, pressed);

        if (hovered && mouse.leftPressed)
        {
            PlayMenuClick(audio, settings);
            currentScreen = gReturnScreen;
            gNeedsReset   = true;
        }
    }

    if (IsKeyPressed(KEY_ESCAPE))
    {
        currentScreen = gReturnScreen;
        gNeedsReset   = true;
    }
}

void DrawSettingUI(
    Font fontTitle,
    Font fontSmall,
    Font fontMini,
    const MouseState& mouse,
    const AppSettings& settings
)
{
    DrawBackgroundOnly();

    //  Panel
    Rectangle panel = { PANEL_X, PANEL_Y, PANEL_W, PANEL_H };
    DrawPanelFrame(panel);

    //  "SETTINGS" title — fontSmall loaded at 28px, draw at native 28px
    DrawCenteredInRange(fontSmall, "SETTINGS",
        PANEL_X, PANEL_X + PANEL_W, PANEL_Y + 14.0f, 28.0f,
        Color{ 255, 235, 225, 255 });


    //  Sliders
    // Indices: 0=musicVolume, 1=sfxVolume, 2=uiBrightness
    const char* sldLabel[3]        = { "MUSIC", "SOUND EFFECT", "BRIGHTNESS" };
    const SettingTex* sldBtnTex[3] = { &gBtnMusic, &gBtnSFX, &gBtnPlain };
    float sldValues[3]             = {
        settings.musicVolume,
        settings.sfxVolume,
        settings.uiBrightness
    };

    for (int i = 0; i < 3; ++i)
    {
        bool over = IsMouseOverRect(mouse, SliderHit(SLD_TRACK_X[i], SLD_CENTER_Y[i]));
        // fontMini (16px) for button labels — drawn at native 16px
        DrawLabelButton(*sldBtnTex[i], sldLabel[i], SLD_BTN_X[i], SLD_CENTER_Y[i], fontSmall, over);
        DrawSliderControl(SLD_TRACK_X[i], SLD_CENTER_Y[i], sldValues[i], gActiveDrag == i);
    }

    //  Toggles
    // Indices: 0=highlightLastMove
    const char* togLabel[1] = { "HIGHLIGHT" };

    for (int i = 0; i < 1; ++i)
    {
        bool over = IsMouseOverRect(mouse, ToggleRowHit(TOG_BTN_X[i], TOG_CENTER_Y[i]));
        DrawLabelButton(gBtnPlain, togLabel[i], TOG_BTN_X[i], TOG_CENTER_Y[i], fontSmall, over);
        DrawToggleControl(TOG_TRACK_X[i], TOG_CENTER_Y[i], gToggleAnim[i]);
    }

    //  BACK button (animIndex = 30) — fontSmall at native 28px
    {
        Rectangle hitRect = GetButtonRect(gSettingButtons[0]);
        bool hovered = IsMouseOverRect(mouse, hitRect);
        bool pressed = hovered && mouse.leftDown;
        DrawUIButton(30, gSettingButtons[0], fontSmall, hovered, pressed);
    }
}
