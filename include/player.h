#ifndef PLAYER
#define PLAYER

#include "component.h"
#include "eventListener.h"
#include "collision.h"
#include "taskListGlobal.h"

int spawnPlayer(int playerX, int playerY, int playerZ, enum Direction playerDir, enum Direction keyDir);

void viewportMoveTowardTarget(int speedX, int speedY);

void updateViewport();

void taskMoveDispatch(int entId, Task* task);

void getXZTribools(enum Direction dir, int fwdBwdTribool, int* xTribool, int* zTribool);

void taskMove(int entId, Task* task);

/** dir must be 1 or -1 */
void moveEnt(int entId, Task* task, int dir);

void taskTurnDispatch(int entId, Task* task);

void taskTurn(int entId, Task* task);

void taskChangeLevel(int entId, Task * task);

void turnEnt(int entId, Task* task, int amount);

void updatePlayerStuff();

#endif
