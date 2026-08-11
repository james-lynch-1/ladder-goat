#ifndef GRAPHICSUTIL
#define GRAPHICSUTIL

#include "global.h"
#include "collision.h"

PositionMini getScreenPos(Position pos);

void initSpriteCells();

void drawSpriteCells(Position pos, bool isCellsRightToLeft);

#endif
