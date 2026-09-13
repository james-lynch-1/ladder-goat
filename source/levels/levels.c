#include "levels.h"

int getNextLevelIndex() {
    int numLevels = sizeof(gLevels) / sizeof(LevelData*);
    return gLevelData->levelId == numLevels - 1 ? 0 : gLevelData->levelId + 1;
}

void changeLevel(int levelId) {
    gLevelData = gLevels[levelId];
    memset32(&gColl, 0, sizeof(gColl) / 4);

    int playerGroupId = -1;
    for (int i = 0; i < numComps(COMP_GROUP); i++) {
        if ((gGroupCompsDense[i].memberIds[0] == gPlayerId) || (gGroupCompsDense[i].memberIds[0] == gLadderId)) {
            playerGroupId = gGroupCompsDense[i].header.entId;
            break;
        }
    }
    int i = 0, j = 0;
    while (i < gNumEnts && j < MAX_ENTS) {
        if (gEntFlags[i]) {
            if (i != gPlayerId && i != gLadderId && i != playerGroupId)
                markEntToBeDeleted(i++);
            else i++;
        }
        j++;
    }
    deleteMarkedEnts();
    gNumListenersPerType[COMP_SLAPPABLE] = 0;

    setPhysPosAndDir(gPlayerId,
        (gLevelData->playerPos.x * 16) << 16,
        (gLevelData->playerPos.y * 16) << 16,
        (gLevelData->playerPos.z * 16) << 16,
        gLevelData->playerDir);
    setPhysPosAndDir(gLadderId,
        (gLevelData->ladderPos.x * 16) << 16,
        (gLevelData->ladderPos.y * 16) << 16,
        (gLevelData->ladderPos.z * 16) << 16,
        gLevelData->playerDir);
    turnEnt(gPlayerId, 0, 0);
    turnEnt(gLadderId, 0, 0);

    updateZDepth(getComponent(gPlayerId, COMP_OBJ));

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

    updateObj(gPlayerId);
    updateObj(gLadderId);
    // TODO: change bg layer, when we have those
}

const LevelData* gLevels[3] = {
    &level0,
    &level1,
    &level2
};
