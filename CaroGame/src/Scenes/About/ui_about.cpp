#include "Scenes/About/ui_about.h"
#include "View/ui_background.h"
#include "View/ui_button.h"
#include "View/ui_frame.h"
#include "Model/config.h"

// --- Layout constants ---
static constexpr float PANEL_W      = 590.0f;
static constexpr float PANEL_H      = 570.0f;
static constexpr float PANEL_X      = SCREEN_WIDTH  * 0.5f - PANEL_W * 0.5f;
static constexpr float PANEL_Y      = 155.0f;

// Title bar ("ABOUT US") sits on top of the panel
static constexpr float TITLE_BAR_H  = 62.0f;
static constexpr float TITLE_BAR_Y  = PANEL_Y;

// Inner content starts below the title bar
static constexpr float CONTENT_X    = PANEL_X  + 18.0f;
static constexpr float CONTENT_Y    = PANEL_Y  + TITLE_BAR_H + 14.0f;
static constexpr float CONTENT_W    = PANEL_W  - 36.0f;

// Card (member row) dimensions
static constexpr float CARD_H       = 58.0f;
static constexpr float CARD_GAP     = 4.0f;
static constexpr float BADGE_SIZE   = 50.0f;
static constexpr float BADGE_PAD    = 8.0f;   // padding inside card before badge

// Colors
static constexpr Color COL_TITLE_BG    = { 130, 30,  55, 255 };  // deep red title bar
static constexpr Color COL_TITLE_TEXT  = { 235, 225, 210, 255 };
static constexpr Color COL_GOLD        = { 210, 170,  80, 255 };
static constexpr Color COL_CARD_BG     = { 105,  95,  75, 220 };
static constexpr Color COL_BADGE_BG    = { 155,  30,  55, 255 };
static constexpr Color COL_BADGE_TEXT  = { 235, 225, 210, 255 };
static constexpr Color COL_ID_TEXT     = { 200, 185, 155, 255 };
static constexpr Color COL_NAME_TEXT   = { 235, 225, 210, 255 };
static constexpr Color COL_DECO_SQ    = { 185,  35,  65, 255 };

struct MemberInfo {
    const char* initials;
    const char* id;
    const char* name;
};

// Vietnamese UTF-8: strings are split at hex boundaries to prevent MSVC greedy-parsing of hex escapes
static const char MNAME_0[] = "L\xc3\xaa Nguy\xe1\xbb" "\x85n Th\xc3\xb9y Linh";
static const char MNAME_1[] = "Tr\xe1\xba\xa7n Th\xe1\xbb" "\x8b L\xe1\xbb\xa3i";
static const char MNAME_2[] = "Nguy\xe1\xbb" "\x85n Tr\xe1\xba\xa7n Minh Tr\xc3\xad";
static const char MNAME_3[] = "Hu\xe1\xbb\xb3nh Tr\xe1\xba\xa7n Ph\xc6\xb0\xe1\xbb" "\x9b" "c Thi\xe1\xbb" "\x87n";
static const char MNAME_4[] = "\xc3\x82u B\xe1\xba\xa3o Tr\xc3\xa2n";

static MemberInfo MEMBERS[] = {
    { "TL", "24120085", MNAME_0 },
    { "TL", "24120370", MNAME_1 },
    { "MT", "24120471", MNAME_2 },
    { "PT", "24120454", MNAME_3 },
    { "BT", "24120468", MNAME_4 },
};
static constexpr int MEMBER_COUNT = 5;

static void DrawCentered(Font font, const char* text, float y, float fontSize, Color color, float spacing = 2.0f)
{
    if (!text || text[0] == '\0') return;
    Vector2 sz = MeasureTextEx(font, text, fontSize, spacing);
    DrawTextEx(font, text,
        Vector2{ SCREEN_WIDTH * 0.5f - sz.x * 0.5f, y },
        fontSize, spacing, color);
}

static void DrawMemberCard(Font fontTitle, Font fontSmall, float cardY, const MemberInfo& m)
{
    // Card background
    Rectangle cardRect = { CONTENT_X, cardY, CONTENT_W, CARD_H };
    DrawRectangleRounded(cardRect, 0.15f, 8, COL_CARD_BG);
    DrawRectangleRoundedLinesEx(cardRect, 0.15f, 8, 1.5f, Color{ 160, 145, 110, 180 });

    // Avatar badge (red square with initials)
    Rectangle badgeRect = {
        CONTENT_X + BADGE_PAD,
        cardY + (CARD_H - BADGE_SIZE) * 0.5f,
        BADGE_SIZE,
        BADGE_SIZE
    };
    DrawRectangleRounded(badgeRect, 0.2f, 8, COL_BADGE_BG);

    Vector2 initSz = MeasureTextEx(fontTitle, m.initials, 22.0f, 1.0f);
    DrawTextEx(fontTitle, m.initials,
        Vector2{
            badgeRect.x + badgeRect.width  * 0.5f - initSz.x * 0.5f,
            badgeRect.y + badgeRect.height * 0.5f - initSz.y * 0.5f - 3.0f
        },
        22.0f, 1.0f, COL_BADGE_TEXT);

    // Text area: student ID (small, on top) + name (bigger, below)
    float textX = badgeRect.x + BADGE_SIZE + 14.0f;
    float idY   = cardY + CARD_H * 0.5f - 22.0f;
    float nameY = cardY + CARD_H * 0.5f - 2.0f;

    DrawTextEx(fontSmall, m.id,   Vector2{ textX, idY },   22.0f, 1.5f, COL_ID_TEXT);
    DrawTextEx(fontTitle, m.name, Vector2{ textX, nameY }, 24.0f, 1.5f, COL_NAME_TEXT);
}

void InitAboutUI() {}
void ShutdownAboutUI() {}

void UpdateAboutUI(
    const MouseState& mouse,
    float dt,
    AudioAssets& audio,
    const AppSettings& settings,
    ScreenState& currentScreen
)
{
    bool hovered = false, pressed = false;
    UpdateUIButton(31, gAboutButtons[0], mouse, dt, audio, settings, hovered, pressed);

    if (hovered && mouse.leftPressed)
    {
        PlayMenuClick(audio, settings);
        currentScreen = SCREEN_MAIN_MENU;
    }

    if (IsKeyPressed(KEY_ESCAPE))
        currentScreen = SCREEN_MAIN_MENU;
}

void DrawAboutUI(Font fontTitle, Font fontSmall, const MouseState& mouse)
{
    DrawBackgroundOnly();

    // --- Outer panel frame ---
    DrawPanelFrame({ PANEL_X, PANEL_Y, PANEL_W, PANEL_H });

    // --- Title bar: "ABOUT US" ---
    {
        Rectangle titleBar = { PANEL_X, TITLE_BAR_Y, PANEL_W, TITLE_BAR_H };
        DrawRectangleRec(titleBar, COL_TITLE_BG);

        // Decorative small squares on each side of title
        float sqSize = 16.0f;
        float sqY    = TITLE_BAR_Y + TITLE_BAR_H * 0.5f - sqSize * 0.5f;
        DrawRectangleRec({ PANEL_X + 18.0f, sqY, sqSize, sqSize }, COL_DECO_SQ);
        DrawRectangleRec({ PANEL_X + PANEL_W - 18.0f - sqSize, sqY, sqSize, sqSize }, COL_DECO_SQ);

        DrawCentered(fontTitle, "ABOUT US",
            TITLE_BAR_Y + TITLE_BAR_H * 0.5f - 16.0f,
            28.0f, COL_TITLE_TEXT);
    }

    // --- University card ---
    float curY = CONTENT_Y;
    {
        Rectangle uniCard = { CONTENT_X, curY, CONTENT_W, CARD_H };
        DrawRectangleRounded(uniCard, 0.15f, 8, COL_CARD_BG);
        DrawRectangleRoundedLinesEx(uniCard, 0.15f, 8, 1.5f, Color{ 160, 145, 110, 180 });

        // "U" badge
        Rectangle badgeRect = {
            CONTENT_X + BADGE_PAD,
            curY + (CARD_H - BADGE_SIZE) * 0.5f,
            BADGE_SIZE, BADGE_SIZE
        };
        DrawRectangleRounded(badgeRect, 0.2f, 8, COL_BADGE_BG);
        Vector2 uSz = MeasureTextEx(fontTitle, "U", 26.0f, 1.0f);
        DrawTextEx(fontTitle, "U",
            Vector2{
                badgeRect.x + badgeRect.width  * 0.5f - uSz.x * 0.5f,
                badgeRect.y + badgeRect.height * 0.5f - uSz.y * 0.5f - 3.0f
            },
            26.0f, 1.0f, COL_BADGE_TEXT);

        // University name centered vertically in card
        float textX = badgeRect.x + BADGE_SIZE + 14.0f;
        float textY = curY + CARD_H * 0.5f - 14.0f;
        DrawTextEx(fontTitle, "University of Science - VNUHCM",
            Vector2{ textX, textY }, 22.0f, 1.5f, COL_NAME_TEXT);
    }
    curY += CARD_H + 10.0f;

    // --- "* CARO GAME PROJECT *" ---
    DrawCentered(fontTitle, "* CARO GAME PROJECT *", curY, 22.0f, COL_GOLD);
    curY += 32.0f;

    // --- Dashed separator ---
    {
        float dashW = 8.0f, dashH = 2.0f, dashGap = 6.0f;
        float sepX = CONTENT_X + 10.0f;
        float sepEndX = CONTENT_X + CONTENT_W - 10.0f;
        float sepY = curY;
        for (float x = sepX; x < sepEndX; x += dashW + dashGap)
        {
            float w = (x + dashW > sepEndX) ? (sepEndX - x) : dashW;
            DrawRectangleRec({ x, sepY, w, dashH }, Color{ 180, 165, 120, 180 });
        }
    }
    curY += 14.0f;

    // --- "[ Game developed by ]" ---
    DrawCentered(fontSmall, "[ Game developed by ]", curY, 22.0f, COL_GOLD);
    curY += 34.0f;

    // --- Member cards ---
    for (int i = 0; i < MEMBER_COUNT; ++i)
    {
        DrawMemberCard(fontTitle, fontSmall, curY, MEMBERS[i]);
        curY += CARD_H + CARD_GAP;
    }

    // --- BACK button ---
    Rectangle hitRect = GetButtonRect(gAboutButtons[0]);
    bool hov = IsMouseOverRect(mouse, hitRect);
    bool prs = hov && mouse.leftDown;
    DrawUIButton(31, gAboutButtons[0], fontTitle, hov, prs);
}
