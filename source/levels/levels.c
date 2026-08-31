#include "levels.h"

void changeLevel(int levelId) {
    gLevelData = gLevels[levelId];
    memcpy32(&gColl, gLevelData->clsn, sizeof(CollLayer) * gLevelData->yHeight / 4);
    int playerGroupId = gPlayerId != -1 ?
        ((MemberComponent*)getComponent(gPlayerId, COMP_MEMBER))->groupIds[0] :
        -1;
    deleteEnt(gPlayerId);
    deleteEnt(gLadderId);
    deleteEnt(playerGroupId);
    gPlayerId = gLadderId = -1;
    deallocateSpriteCells();

    drawSpriteCells();
    spawnPlayer(
        gLevelData->ladderPos.x,
        gLevelData->ladderPos.y,
        gLevelData->ladderPos.z,
        gLevelData->playerPos.x,
        gLevelData->playerPos.y,
        gLevelData->playerPos.z
    );
    // TODO: change bg layer, when we have those
}

const LevelData* gLevels[] = {
    &level0,
    &level1
};
