#include "entity.h"

int spawnEntity(int entityKind, int x, int y) {
    return 0;
}

int reserveEntSlot() {
    if (gNumEnts == MAX_ENTS) return -1;
    int i = 0;
    while (gEntFlags[i] != 0) i++;
    gEntFlags[i] = 1;
    gNumEnts++;
    return i;
}

void markEntToBeDeleted(int index) {
    if (index == -1) return;
    gEntsToDelete[gNumEntsToDelete++] = index;
}

int deleteEnt(int index) {
    if (gEntFlags[index] == 0) return 0;
    for (int i = 0; i < NUM_COMP_TYPES; i++) {
        void(*delFn)(int);
        delFn = (void(*)(int))gCompTable[i][COMP_REMOVERS];
        delFn(index);
    }
    gNumEnts--;
    gEntFlags[index] = 0;
    return 1;
}

void deleteMarkedEnts() {
    int numToDelete = gNumEntsToDelete; // because gNumEntsToDelete changes under us
    for (int i = 0; i < numToDelete; i++) {
        deleteEnt(gEntsToDelete[i]);
    }
    gNumEntsToDelete = 0;
}

