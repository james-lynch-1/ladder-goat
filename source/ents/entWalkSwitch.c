#include "ent.h"

// walkswitch callbacks need to access the currentWeight and the weightToAdd to decide what to do

int spawnEntWalkSwitch(int tileX, int tileY, int tileZ, int entFlags, void(*callback)) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, 0, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 1, 0) ||
        !addComponentObj(entId, 0, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(spriteCellFenceNETiles, 512)) |
            ATTR2_PALBANK(PAL_PURPLE),
            8,
            COMP_PHYSICS) ||
        !addComponentWalkable(entId, 0, callback)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}

void walkSwitchCBEnableGoal(int entId) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    if (walk->weightToAdd < 0)
        return;

    if ((walk->currentWeight == 0) && (walk->weightToAdd > 0))
        for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
            WalkableComponent* walk = &gWalkableCompsDense[i];
            if (walk->callback == walkSwitchCBChangeLevel) {
                if (isEntInactive(walk->header.entId))
                    makeEntActive(walk->header.entId);
                else makeEntInactive(walk->header.entId);
                break;
            }
        }
}

void walkSwitchCBChangeLevel(int entId) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    if (walk->weightToAdd > 0) {
        int numLevels = sizeof(gLevels) / sizeof(LevelData*);
        int nextLevel = gLevelData->levelId == numLevels - 1 ? 0 : gLevelData->levelId + 1;
        addTaskToQueue(gPlayerId, TASK_CHANGE_LEVEL, nextLevel);
        return;
    }
}
