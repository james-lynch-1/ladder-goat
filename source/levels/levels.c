#include "levels.h"

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

    setPhysPos(gPlayerId,
        (gLevelData->playerPos.x * 16) << 16,
        (gLevelData->playerPos.y * 16) << 16,
        (gLevelData->playerPos.z * 16) << 16);

    setPhysPos(gLadderId,
        (gLevelData->ladderPos.x * 16) << 16,
        (gLevelData->ladderPos.y * 16) << 16,
        (gLevelData->ladderPos.z * 16) << 16);
    updateObj(gPlayerId);
    updateObj(gLadderId);
    drawSpriteCells();
    for (int i = 0; i < gLevelData->entArrLength; i++)
        spawnEnt(
            gLevelData->entArr[i].entKind,
            gLevelData->entArr[i].tilePos.x,
            gLevelData->entArr[i].tilePos.y,
            gLevelData->entArr[i].tilePos.z,
            gLevelData->entArr[i].entFlags,
            gLevelData->entArr[i].callback
        );

    // TODO: change bg layer, when we have those
}

const LevelData* gLevels[2] = {
    &level0,
    &level1
};
