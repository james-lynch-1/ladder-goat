#include "ent.h"

int spawnEntWalkSwitch(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void(*callback)) {
    int entId = reserveEntSlot();
    int walkFlags = data0;
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
        !addComponentWalkable(entId, walkFlags, data0, data1, data2, data3, callback)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}

void walkSwitchCBOpenGate(int entId) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    CellComponent* cellToDeactivate = gColl[0][10][6];
    volatile int numBtnsPressed = 0;
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        WalkableComponent* walkToCheck = &gWalkableCompsDense[i];
        if ((walkToCheck->callback == walkSwitchCBOpenGate) &&
            (walkToCheck->header.flags & WALK_PRESSED_FLAG))
            numBtnsPressed++;
    }
    if (walk->currentWeight + walk->weightToAdd <= 0) {
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
    if (walk->currentWeight + walk->weightToAdd >= PLAYER_WEIGHT) {
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

void walkSwitchCBEnableGoal(int entId) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    bool wasPressed = walk->header.flags & WALK_PRESSED_FLAG;
    if (walk->currentWeight + walk->weightToAdd <= 1) {
        if (walk->header.flags & WALK_TOGGLE_FLAG)
            return;
        if (wasPressed) {
            walk->header.flags &= ~WALK_PRESSED_FLAG;
            toggleGoalsActive();
            if (obj) {
                stopUsingSprite(obj->attr2 & ATTR2_ID_MASK);
                obj->attr2 &= ~(ATTR2_ID_MASK);
                obj->attr2 |= fetchSprite(walkSwitchTiles, walkSwitchTilesLen);
            }
        }
    }

    if (walk->currentWeight + walk->weightToAdd >= PLAYER_WEIGHT) {
        walk->header.flags = (walk->header.flags & ~WALK_PRESSED_FLAG) | !wasPressed;
        if (obj) {
            stopUsingSprite(obj->attr2 & ATTR2_ID_MASK);
            obj->attr2 &= ~(ATTR2_ID_MASK);
            if (wasPressed)
                obj->attr2 |= fetchSprite(walkSwitchTiles, walkSwitchTilesLen);
            else
                obj->attr2 |= fetchSprite(walkSwitchPressedTiles, walkSwitchPressedTilesLen);
        }
        toggleGoalsActive();
    }
}

void toggleGoalsActive() {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    PositionMini playerTilePos = getTilePos(gPlayerId);
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        WalkableComponent* walk = &gWalkableCompsDense[i];
        if (walk->callback == walkSwitchCBChangeLevel) {
            if (isEntInactive(walk->header.entId)) {
                makeEntActive(walk->header.entId);
                if (isEqualPosMini(playerTilePos, getTilePos(gWalkableCompsDense[i].header.entId)))
                    checkWalkableSteppingOn(&gWalkableCompsDense[i], playerPhys->weight);
            }
            else makeEntInactive(walk->header.entId);
            break;
        }
    }
}

void activateGoals() {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    PositionMini playerTilePos = getTilePos(gPlayerId);
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        WalkableComponent* walk = &gWalkableCompsDense[i];
        if (walk->callback == walkSwitchCBChangeLevel) {
            if (isEntInactive(walk->header.entId)) {
                makeEntActive(walk->header.entId);
                if (isEqualPosMini(playerTilePos, getTilePos(gWalkableCompsDense[i].header.entId)))
                    checkWalkableSteppingOn(&gWalkableCompsDense[i], playerPhys->weight);
            }
            break;
        }
    }
}

void walkSwitchCBChangeLevel(int entId) {
    WalkableComponent* walk = getComponent(entId, COMP_WALKABLE);
    RotationComponent* rot = getComponent(entId, COMP_ROTATION);
    TaskQueueComponent* tQ = getComponent(entId, COMP_TASK_QUEUE);
    if (walk->weightToAdd >= PLAYER_WEIGHT) {
        if (rot && tQ)
            addTaskToQueue(entId, TASK_SPIN_ENDLESSLY, 0x4);
        int nextLevel = getNextLevelIndex();
        addTaskToQueue(gPlayerId, TASK_CHANGE_LEVEL, nextLevel);
        removeComponentInput(gPlayerId);
        return;
    }
}
