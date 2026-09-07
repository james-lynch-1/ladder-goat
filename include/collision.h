#ifndef COLLISION
#define COLLISION

#include "global.h"

int getClsnVal(CellComponent * cell);

int getClsnValAtTilePos(PositionMini tilePos);

/** Returns the value in the collision map of the tile the player is moving towards.
 * FWD direction is 1, BWD direction is -1 (supply the key_tri_vert() value)
 */
int checkCollisionMove(PhysicsComponent* phys, int fwdBwdTribool);

/** Returns whether the plyer is going to collide with a tile when they rotate.
 * CW direction is -1, CCW direction is 1 (supply the key_tri_horz() value)
 */
int checkCollisionTurn(PhysicsComponent* phys, int turnDir);

void getXZOffsets(int angle, Hitbox hitbox, int* xOffsPos, int* xOffsNeg, int* zOffsPos, int* zOffsNeg);

void checkWalkables(int weight, PositionMini tilePos, PositionMini nextTilePos, int xOffsPos, int xOffsNeg, int zOffsPos, int zOffsNeg, int turnDir);

void checkWalkableSteppingOn(WalkableComponent * walk, int weight);

void checkWalkableSteppingOff(WalkableComponent* walk, int weight);

#endif
