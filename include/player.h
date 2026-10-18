#ifndef PLAYER
#define PLAYER

#include "component.h"
#include "eventListener.h"
#include "collision.h"

int spawnPlayer(int playerX, int playerY, int playerZ, enum Direction playerDir, enum Direction keyDir);

void updateViewport();

void taskMoveDispatch(int entId, Task* task);

void taskMove(int entId, Task* task);

/** dir must be 1 or -1 */
void moveEnt(int entId, Task* task, int dir);

void taskTurnDispatch(int entId, Task* task);

void taskTurn(int entId, Task* task);

void taskChangeLevel(int entId, Task * task);

void turnEnt(int entId, Task* task, int amount);

void updatePlayerStuff();

#endif
