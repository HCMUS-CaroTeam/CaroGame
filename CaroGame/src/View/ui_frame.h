#pragma once
#include "raylib.h"

void InitUIFrameSystem();
void ShutdownUIFrameSystem();

void DrawPanelFrame(Rectangle dest);
void DrawSmallFrame(Rectangle dest);
void DrawCardFrame(Rectangle dest);

bool IsUIFrameReady();

/*
#include "View/ui_frame.h"

DrawPanelFrame({ x, y, width, height });
DrawSmallFrame({ x, y, width, height });
DrawCardFrame({ x, y, width, height });
*/