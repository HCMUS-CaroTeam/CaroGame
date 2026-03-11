#pragma once
#include "raylib.h"
// Các hàm tiện ích để xử lý màu sắc và vẽ hiệu ứng UI
Color FadeColorEx(Color c, float alpha);
// Tăng độ sáng của một màu lên một lượng nhất định, không vượt quá 255
Color AddColor(Color c, int amount);
// Vẽ một hình chữ nhật với hiệu ứng glow xung quanh
void DrawGlowRect(Rectangle rect, Color color, int layers, float roundness, float scaleStep);
//	Vẽ một hình chữ nhật với hiệu ứng viền sáng xung quanh
void DrawCornerBrackets(Rectangle rect, Color color, float alpha);
// Vẽ một hình chữ nhật với hiệu ứng scanline chạy qua
void DrawScanLines(Rectangle rect, float alpha, float offset);

// Vẽ phần nền của một nút với hiệu ứng gradient và pattern
void DrawCutCornerFrame(Rectangle rect, float cut, float thickness, Color color, float alpha);
// Vẽ phần nền của một nút với hiệu ứng gradient và pattern
void DrawHudPattern(Rectangle rect, Color color, float alpha, float spacing);
// Vẽ hiệu ứng quét sáng khi hover lên một nút
void DrawHoverSweep(Rectangle rect, Color color, float alpha, float t);