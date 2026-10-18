#include "ent.h"

int spawnEntWalkSwitch(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void* callback) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    int walkFlags = data0;
    int threshold = data1;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, 0, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 1, 0) ||
        !addComponentObj(entId, 0, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(walkSwitchTiles, 512)) |
            ATTR2_PALBANK(PAL_BLUE),
            8,
            COMP_PHYSICS) ||
        !addComponentWalkable(entId, walkFlags, threshold, callback)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}

void walkSwitchCBOpenGate(int entId, int weight) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    CellComponent* cellToDeactivate = gColl[0][8][4];
    bool wasPressed = walk->currentWeight >= walk->threshold;
    bool isPressed = walk->currentWeight + weight >= walk->threshold;
    int numBtnsPressed = 0;
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        WalkableComponent* walkToCheck = &gWalkableCompsDense[i];
        if ((walkToCheck->callback == walkSwitchCBOpenGate) &&
            (walkToCheck->header.flags & WALK_PRESSED_FLAG))
            numBtnsPressed++;
    }
    if (wasPressed && !isPressed) {
        if ((walk->callback == walkSwitchCBOpenGate) &&
            (walk->header.flags & WALK_PRESSED_FLAG))
            numBtnsPressed--;
        walk->header.flags &= ~WALK_PRESSED_FLAG;
        if (obj) {
            stopUsingSprite(obj->attr2 & ATTR2_ID_MASK);
            obj->attr2 &= ~(ATTR2_ID_MASK);
            obj->attr2 |= fetchSprite(walkSwitchTiles, walkSwitchTilesLen);
        }
        if (numBtnsPressed < 2)
            makeEntActive(cellToDeactivate->header.entId);
        return;
    }
    if (!wasPressed && isPressed) {
        numBtnsPressed++;
        walk->header.flags |= WALK_PRESSED_FLAG;
        if (obj) {
            stopUsingSprite(obj->attr2 & ATTR2_ID_MASK);
            obj->attr2 &= ~(ATTR2_ID_MASK);
            obj->attr2 |= fetchSprite(walkSwitchPressedTiles, walkSwitchPressedTilesLen);
        }
        if (numBtnsPressed >= 2) {
            // todo: add a smooth opening animation for the gate
            // TaskQueueComponent* cellTQ = getComponent(cell->header.entId, COMP_TASK_QUEUE);
            // if (!cellTQ)
            //     cellTQ = addComponentTaskQueue(cell->header.entId, 0);
            makeEntInactive(cellToDeactivate->header.entId);
        }
    }
}

void walkSwitchCBEnableGoal(int entId, int weight) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    bool wasPressed = (walk->header.flags & WALK_PRESSED_FLAG) != 0;
    bool isSteppingOn = walk->currentWeight < walk->threshold && walk->currentWeight + weight >= walk->threshold;
    bool isSteppingOff = walk->currentWeight + weight < walk->threshold;
    bool isSwitching = false;
    if (isSteppingOff && wasPressed) {
        if (walk->header.flags & WALK_TOGGLE_FLAG)
            return;
        walk->header.flags &= ~WALK_PRESSED_FLAG;
        isSwitching = true;
    }
    if (isSteppingOn) {
        if (walk->header.flags & WALK_TOGGLE_FLAG)
            walk->header.flags ^= WALK_PRESSED_FLAG;
        else
            walk->header.flags |= WALK_PRESSED_FLAG;
        isSwitching = true;
    }
    if (isSwitching) {
        const u16* tiles = walk->header.flags & WALK_PRESSED_FLAG ? walkSwitchPressedTiles : walkSwitchTiles;
        toggleGoalsActive();
        if (obj) {
            stopUsingSprite(obj->attr2 & ATTR2_ID_MASK);
            obj->attr2 &= ~(ATTR2_ID_MASK);
            obj->attr2 |= fetchSprite(tiles, walkSwitchTilesLen);
        }
    }

}

void walkSwitchCBChangeLevel(int entId, int weight) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    RotationComponent* rot = getComponent(entId, COMP_ROTATION);
    TaskQueueComponent* tQ = getComponent(entId, COMP_TASK_QUEUE);
    bool isSteppingOn = walk->currentWeight + weight >= walk->threshold;
    if (isSteppingOn) {
        if (rot && tQ)
            addTaskToQueue(entId, TASK_SPIN_ENDLESSLY, 0x4);
        int nextLevel = getNextLevelIndex();
        addTaskToQueue(gPlayerId, TASK_CHANGE_LEVEL, nextLevel);
        return;
    }
}

void toggleGoalsActive() {
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        WalkableComponent* walk = &gWalkableCompsDense[i];
        if (walk->callback == walkSwitchCBChangeLevel) {
            if (isEntInactive(walk->header.entId)) {
                makeEntActive(walk->header.entId);
            }
            else makeEntInactive(walk->header.entId);
            break;
        }
    }
}

void activateGoals() {
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        WalkableComponent* walk = &gWalkableCompsDense[i];
        if (walk->callback == walkSwitchCBChangeLevel) {
            if (isEntInactive(walk->header.entId)) {
                makeEntActive(walk->header.entId);
            }
            break;
        }
    }
}
