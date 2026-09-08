#ifndef ENT
#define ENT

#include "global.h"

int spawnEnt(int entityKind, int x, int y, int z, int entFlags, int moveTimerLength, void* data);

int spawnEntWalkSwitch(int tileX, int tileY, int tileZ, int entFlags, int moveTimerLength, void(*callback));

void walkSwitchCBEnableGoal(int entId);

void walkSwitchCBChangeLevel(int entId);

int spawnEntSlapSwitch(int tileX, int tileY, int tileZ, int entFlags, int moveTimerLength, void(*callback));

void slapSwitchStartSpinning(int entId);

void slapSwitchCBEnableGoal(int entId);

void eLMakeGoalInactive(EventListener* eL, int entId);

void eLDecrementSpinTaskSpeed(EventListener* eL, int entId);

void taskSpinEndlessly(int entId, Task* task);

int spawnEntGoal(int tileX, int tileY, int tileZ, int entFlags, int moveTimerLength, void(*callback));

/** Returns -1 if no slot available */
int reserveEntSlot();

void makeEntInactive(int entId);

void makeEntActive(int entId);

bool isEntInactive(int entId);

/** Marks an ent to be deleted at the end of the frame. */
void markEntToBeDeleted(int entId);

int deleteEnt(int entId);

void deleteMarkedEnts();

#endif
