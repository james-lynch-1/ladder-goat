#ifndef PLAYER
#define PLAYER

#include "component.h"
#include "eventListener.h"
#include "enemy.h"
#include "collision.h"

/** Spawns player with ladder */

int spawnPlayer(int ladderX, int ladderY, int ladderZ, int playerX, int playerY, int playerZ);

void taskMove(int entId, Task* task);

void taskMovePlayerAndLadder(int entId, Task* task);

void taskMovePlayer(int entId, Task* task);

void taskMoveLadder(int entId, Task* task);

/** dir must be 1 or -1 */
void moveEnt(int entId, Task* task, int dir);

void taskTurn(int entId, Task* task);

void turnLadder(int entId, Task * task, int dir);

void turnPlayer(int entId, Task* task, int dir);

void updatePlayerStuff();

void handlePlayerDied();

#endif
