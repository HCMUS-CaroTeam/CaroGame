#include "ui_fx.h"
#include <algorithm>

Color FadeColorEx(Color c, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    if (alpha > 1.0f) alpha = 1.0f;
    c.a = static_cast<unsigned char>(255.0f * alpha);
    return c;
}

Color AddColor(Color c, int amount)
{
    c.r = static_cast<unsigned char>(std::min(255, c.r + amount));
    c.g = static_cast<unsigned char>(std::min(255, c.g + amount));
    c.b = static_cast<unsigned char>(std::min(255, c.b + amount));
    return c;
}

void DrawGlowRect(Rectangle rect, Color color, int layers, float roundness, float scaleStep)
{
    for (int i = layers; i >= 1; --i)
    {
        float ex = static_cast<float>(i) * scaleStep;
        Rectangle r{
            rect.x - ex,
            rect.y - ex,
            rect.width + ex * 2.0f,
            rect.height + ex * 2.0f
        };

        float alpha = 0.010f + static_cast<float>(i) * 0.008f;
        DrawRectangleRounded(r, roundness, 12, FadeColorEx(color, alpha));
    }
}

void DrawCornerBrackets(Rectangle rect, Color color, float alpha)
{
    Color c = FadeColorEx(color, alpha);
    float x = rect.x;
    float y = rect.y;
    float w = rect.width;
    float h = rect.height;
    float len = 16.0f;
    float t = 2.0f;

    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(len), static_cast<int>(t), c);
    DrawRectangle(static_cast<int>(x), static_cast<int>(y), static_cast<int>(t), static_cast<int>(len), c);

    DrawRectangle(static_cast<int>(x + w - len), static_cast<int>(y), static_cast<int>(len), static_cast<int>(t), c);
    DrawRectangle(static_cast<int>(x + w - t), static_cast<int>(y), static_cast<int>(t), static_cast<int>(len), c);

    DrawRectangle(static_cast<int>(x), static_cast<int>(y + h - t), static_cast<int>(len), static_cast<int>(t), c);
    DrawRectangle(static_cast<int>(x), static_cast<int>(y + h - len), static_cast<int>(t), static_cast<int>(len), c);

    DrawRectangle(static_cast<int>(x + w - len), static_cast<int>(y + h - t), static_cast<int>(len), static_cast<int>(t), c);
    DrawRectangle(static_cast<int>(x + w - t), static_cast<int>(y + h - len), static_cast<int>(t), static_cast<int>(len), c);
}

void DrawScanLines(Rectangle rect, float alpha, float offset)
{
    Color c1 = FadeColorEx(Color{ 255,255,255,255 }, alpha * 0.28f);
    Color c2 = FadeColorEx(Color{ 120,220,255,255 }, alpha * 0.20f);

    int startY = static_cast<int>(rect.y + offset);
    int endY = static_cast<int>(rect.y + rect.height);

    for (int y = startY; y < endY; y += 6)
    {
        DrawLine(
            static_cast<int>(rect.x + 10),
            y,
            static_cast<int>(rect.x + rect.width - 10),
            y,
            ((y / 6) % 2 == 0) ? c1 : c2
        );
    }
}

void DrawCutCornerFrame(Rectangle rect, float cut, float thickness, Color color, float alpha)
{
    Color c = FadeColorEx(color, alpha);

    Vector2 p1{ rect.x + cut, rect.y };
    Vector2 p2{ rect.x + rect.width - cut, rect.y };
    Vector2 p3{ rect.x + rect.width, rect.y + cut };
    Vector2 p4{ rect.x + rect.width, rect.y + rect.height - cut };
    Vector2 p5{ rect.x + rect.width - cut, rect.y + rect.height };
    Vector2 p6{ rect.x + cut, rect.y + rect.height };
    Vector2 p7{ rect.x, rect.y + rect.height - cut };
    Vector2 p8{ rect.x, rect.y + cut };

    DrawLineEx(p1, p2, thickness, c);
    DrawLineEx(p2, p3, thickness, c);
    DrawLineEx(p3, p4, thickness, c);
    DrawLineEx(p4, p5, thickness, c);
    DrawLineEx(p5, p6, thickness, c);
    DrawLineEx(p6, p7, thickness, c);
    DrawLineEx(p7, p8, thickness, c);
    DrawLineEx(p8, p1, thickness, c);
}

void DrawHudPattern(Rectangle rect, Color color, float alpha, float spacing)
{
    Color c = FadeColorEx(color, alpha);

    for (float x = rect.x - rect.height; x < rect.x + rect.width; x += spacing)
    {
        Vector2 a{ x, rect.y + rect.height };
        Vector2 b{ x + rect.height, rect.y };
        DrawLineEx(a, b, 1.0f, c);
    }
}

void DrawHoverSweep(Rectangle rect, Color color, float alpha, float t)
{
    float width = 36.0f;
    float x = rect.x - width + (rect.width + width * 2.0f) * t;

    Rectangle sweep{ x, rect.y + 8.0f, width, rect.height - 16.0f };
    DrawRectangleGradientH(
        static_cast<int>(sweep.x),
        static_cast<int>(sweep.y),
        static_cast<int>(sweep.width),
        static_cast<int>(sweep.height),
        FadeColorEx(color, 0.0f),
        FadeColorEx(color, alpha)
    );
}