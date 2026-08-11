#ifndef PLAYER
#define PLAYER

#include "component.h"
#include "eventListener.h"
#include "enemy.h"
#include "collision.h"

/** Spawns player with ladder */

int spawnPlayer(int ladderX, int ladderY, int ladderZ, int playerX, int playerY, int playerZ);

void taskMoveForward(Task* task);

void taskMoveBackward(Task* task);

/** dir must be 1 or -1 */
void movePlayer(Task* task, int dir);

void taskTurnLeft(Task* task);

void taskTurnRight(Task* task);

/** dir must be 1 or -1 */
void turnPlayer(Task* task, int dir);

void updatePlayerZDepth();

void updatePlayerStuff();

void handlePlayerDied();

#endif
