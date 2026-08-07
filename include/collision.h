#ifndef COLLISION
#define COLLISION

#include "global.h"

extern CollisionMap colMap;

/** Returns the value in the collision map of the tile the player is moving towards.
 * FWD direction is 1, BWD direction is -1 (supply the key_tri_vert() value)
 */
int checkCollisionMove(PhysicsComponent* phys, int dir);

/** Returns whether the plyer is going to collide with a tile when they rotate.
 * CW direction is -1, CCW direction is 1 (supply the key_tri_horz() value)
 */
int checkCollisionTurn(PhysicsComponent* phys, int dir);

#endif
