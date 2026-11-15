#ifndef COLLISION
#define COLLISION

#include "global.h"

int getClsnValAtTilePos(PositionMini tilePos);

int getClsnOffsFromTilePos(PositionMini tilePos);

void setClsnVal(PositionMini tP, ComponentHeader* comp);

bool isTilePosWithinBounds(PositionMini tP);

PositionMini getNextTilePos(PositionMini tP, int fwdBwdTribool);

void setClsnDataActionCacheIndex(PositionMini tilePos, int idx);

void addAction(PhysicsComponent* phys, int flags, PositionMini startPos, PositionMini endPos);

bool addTurn(int entId, bool isCW);

bool addMove(int entId, int fwdBwdTribool);

void addEnteredFlagsToClsnData(PositionMini startPos, PositionMini endPos);

void processActions();

void doMove(PhysicsComponent* phys, Action* move);

void invalidateChildMoves(int entId);

void invalidateSiblingAndParentMoves(int entId);

bool doFalseThings(Action* action);

bool processAction(PositionMini tP);

/** Returns whether the plyer is going to collide with a tile when they rotate.
 * CW direction is -1, CCW direction is 1 (supply the key_tri_horz() value)
 */
int checkCollisionTurn(PhysicsComponent* phys, int turnDir);

void getXZOffsets(int angle, Hitbox hitbox, int* xOffsPos, int* xOffsNeg, int* zOffsPos, int* zOffsNeg);

void checkWalkables(int weight, PositionMini tilePos, PositionMini nextTilePos, int xOffsPos, int xOffsNeg, int zOffsPos, int zOffsNeg, int turnDir);

void checkWalkableSteppingOn(WalkableComponent * walk, int weight);

#endif
