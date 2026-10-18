#include "levels.h"

int getNextLevelIndex() {
    int numLevels = sizeof(gLevels) / sizeof(LevelData*);
    return gLevelData->levelId == numLevels - 1 ? 0 : gLevelData->levelId + 1;
}

void changeLevel(int levelId) {
    gLevelData = gLevels[levelId];
    memset32(&gColl, 0, sizeof(gColl) / 4);
    int i = 0, j = 0;
    while (i < gNumEnts && j < MAX_ENTS) {
        if (gEntFlags[i]) {
            if (i != gPlayerId)
                markEntToBeDeleted(i);
            i++;
        }
        j++;
    }
    deleteMarkedEnts();
    gNumListenersPerType[COMP_SLAPPABLE] = 0;

    ObjComponent* playerObj = getComponent(gPlayerId, COMP_OBJ);
    playerObj->prevId[0] = playerObj->prevId[1] = -1;
    playerObj->nextId[0] = playerObj->nextId[1] = INT16_MAX;
    gDeepestObjEntId[0] = gDeepestObjEntId[1] = gPlayerId;

    setPhysPosAndDir(gPlayerId,
        (gLevelData->playerPos.x * 16) << 16,
        (gLevelData->playerPos.y * 16) << 16,
        (gLevelData->playerPos.z * 16) << 16,
        gLevelData->playerPos.dir);
    turnEnt(gPlayerId, NULL, 0);
    gViewport.canMove = gLevelData->vp.canMove;

    drawSpriteCells();

    for (int i = 0; i < gLevelData->entArrLength; i++)
        spawnEnt(
            gLevelData->entArr[i].entKind,
            gLevelData->entArr[i].tilePos.x,
            gLevelData->entArr[i].tilePos.y,
            gLevelData->entArr[i].tilePos.z,
            gLevelData->entArr[i].entFlags,
            gLevelData->entArr[i].data[0],
            gLevelData->entArr[i].data[1],
            gLevelData->entArr[i].data[2],
            gLevelData->entArr[i].data[3],
            gLevelData->entArr[i].callback
        );

    // TODO: change bg layer, when we have those
}

const LevelData* gLevels[3] = {
    &level0,
    &level1,
    &level2
};
