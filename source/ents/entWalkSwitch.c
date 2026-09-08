#include "ent.h"

int spawnEntWalkSwitch(int tileX, int tileY, int tileZ, int entFlags, int moveTimerLength, void(*callback)) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, 0, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 1, 0) ||
        !addComponentObj(entId, 0, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(walkSwitchTiles, 512)) |
            ATTR2_PALBANK(PAL_BLUE),
            16,
            COMP_PHYSICS) ||
        !addComponentWalkable(entId, 0, moveTimerLength, callback)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}

void walkSwitchCBEnableGoal(int entId) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    if (walk->currentWeight + walk->weightToAdd <= 0) {
        return;
    }

    if ((walk->currentWeight == 0) && (walk->weightToAdd > 0)) {
        ObjComponent* obj = getComponent(entId, COMP_OBJ);
        if (obj)
            changePalette(
                obj,
                ((obj->attr2 & ATTR2_PALBANK_MASK) >> ATTR2_PALBANK_SHIFT) == PAL_ORANGE ?
                PAL_BLUE : PAL_ORANGE);
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
}

void walkSwitchCBChangeLevel(int entId) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    if (walk->weightToAdd > 1) {
        int numLevels = sizeof(gLevels) / sizeof(LevelData*);
        int nextLevel = gLevelData->levelId == numLevels - 1 ? 0 : gLevelData->levelId + 1;
        addTaskToQueue(gPlayerId, TASK_CHANGE_LEVEL, nextLevel);
        return;
    }
}
