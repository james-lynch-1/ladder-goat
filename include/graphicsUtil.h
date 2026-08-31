#ifndef GRAPHICSUTIL
#define GRAPHICSUTIL

#include "global.h"
#include "collision.h"

PositionMini getScreenPos(Position pos);

int getZDepth(ObjComponent* obj, bool isNW);

/** This function iterates through the obj linked list and finds an appropriate spot for
 * an obj based on its zDepth.
 * Assumes the ent has an ObjComponent and a Physics/CellComponent.
 */
void updateZDepth(ObjComponent* obj);

void resetObjZDepth(ObjComponent* toBeReset);

void drawSpriteCells();

void deallocateSpriteCells();

#endif
