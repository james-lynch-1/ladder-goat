#ifndef ENTITY
#define ENTITY

#include "global.h"

int spawnEntity(int entityKind, int x, int y);

/** Returns -1 if no slot available */
int reserveEntSlot();

/** Marks an ent to be deleted at the end of the frame. */
void markEntToBeDeleted(int index);

int deleteEnt(int index);

void deleteMarkedEnts();

#endif
