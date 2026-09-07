#include "ent.h"

int spawnEntSlapSwitch(int tileX, int tileY, int tileZ, int entFlags, void(*callback)) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, 0, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 1, 0) ||
        !addComponentObj(entId, 0, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(spriteCellPurpleCrossTiles, 512)) |
            ATTR2_PALBANK(PAL_ORANGE),
            8,
            COMP_PHYSICS) ||
        !addComponentSlappable(entId, 0, callback)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}

void slapSwitchCBChangeLevel(int entId) {
    int numLevels = sizeof(gLevels) / sizeof(LevelData*);
    int nextLevel = gLevelData->levelId == numLevels - 1 ? 0 : gLevelData->levelId + 1;
    addTaskToQueue(gPlayerId, TASK_CHANGE_LEVEL, nextLevel);
    return;
}
