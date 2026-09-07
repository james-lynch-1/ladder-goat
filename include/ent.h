#ifndef ENT
#define ENT

#include "global.h"

int spawnEnt(int entityKind, int x, int y, int z, int entFlags, void* data);

int spawnEntWalkSwitch(int tileX, int tileY, int tileZ, int entFlags, void(*callback));

void walkSwitchCBEnableGoal(int entId);

void walkSwitchCBChangeLevel(int entId);

int spawnEntSlapSwitch(int tileX, int tileY, int tileZ, int entFlags, void(*callback));

void slapSwitchCBChangeLevel(int entId);

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
