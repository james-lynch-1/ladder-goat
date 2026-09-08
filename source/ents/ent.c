#include "ent.h"

int (*gEntSpawners[NUM_ENT_KINDS])(int tileX, int tileY, int tileZ, int entFlags, int moveTimerLength, void(*callback)) = {
    0, spawnEntWalkSwitch, spawnEntSlapSwitch, spawnEntGoal
};

int spawnEnt(int entKind, int x, int y, int z, int entFlags, int moveTimerLength, void* data) {
    return gEntSpawners[entKind](x, y, z, entFlags, moveTimerLength, data);
}

int reserveEntSlot() {
    if (gNumEnts == MAX_ENTS) return -1;
    int i = 0;
    while (gEntFlags[i] != 0) i++;
    gEntFlags[i] = 1;
    gNumEnts++;
    return i;
}

void makeEntInactive(int entId) {
    gEntFlags[entId] |= ENT_INACTIVE;
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    if (obj) hideObj(obj);
}

void makeEntActive(int entId) {
    gEntFlags[entId] = gEntFlags[entId] & ~ENT_INACTIVE;
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    if (obj) unhideObj(obj);
}

bool isEntInactive(int entId) {
    return (gEntFlags[entId] & ENT_INACTIVE) != 0;
}

void markEntToBeDeleted(int entId) {
    if (entId == -1) return;
    gEntsToDelete[gNumEntsToDelete++] = entId;
}

int deleteEnt(int entId) {
    if (entId == -1 || (gEntFlags[entId] == 0)) return 0;
    for (int i = 0; i < NUM_COMP_TYPES; i++) {
        void(*delFn)(int);
        delFn = (void(*)(int))gCompTable[i][COMP_REMOVERS];
        delFn(entId);
    }
    gNumEnts--;
    gEntFlags[entId] = 0;
    return 1;
}

void deleteMarkedEnts() {
    int numToDelete = gNumEntsToDelete; // because gNumEntsToDelete changes under us
    for (int i = 0; i < numToDelete; i++) {
        deleteEnt(gEntsToDelete[i]);
    }
    gNumEntsToDelete = 0;
}

