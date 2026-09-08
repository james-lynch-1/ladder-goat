#include "ent.h"

int spawnEntSlapSwitch(int tileX, int tileY, int tileZ, int entFlags, int moveTimerLength, void(*callback)) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, 0, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 1, 0) ||
        !addComponentObj(entId, 0, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(slapSwitchTiles, 512)) |
            ATTR2_PALBANK(PAL_BLUE),
            8,
            COMP_PHYSICS) ||
        !addComponentSlappable(entId, 0, moveTimerLength, callback) ||
        !addComponentRotation(entId, 0) ||
        !addComponentTaskQueue(entId, 0)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    turnEnt(entId, 0, 0x2000);
    return entId;
}

void slapSwitchStartSpinning(int entId) {
    SlappableComponent* slap = getComponent(entId, COMP_SLAPPABLE);
    TaskQueueComponent* playerTQ = getComponent(gPlayerId, COMP_TASK_QUEUE);
    int turnDir = (playerTQ->queue[playerTQ->head].taskIndex == TASK_TURN) &&
        (playerTQ->queue[playerTQ->head].data == 1) ?
        -1 : 1;
    TaskQueueComponent* switchTQ = getComponent(entId, COMP_TASK_QUEUE);
    int slapForce = slap->moveTimerLength == INT32_MAX ? 6 : slap->moveTimerLength;
    if (!isTaskQueueEmpty(switchTQ))
        switchTQ->queue[switchTQ->head].data = turnDir * slapForce;
    else
        addTaskToQueue(entId, TASK_SPIN_ENDLESSLY, turnDir * slapForce);
}

void slapSwitchCBEnableGoal(int entId) {
    slapSwitchStartSpinning(entId);

    // enable goal
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        if (gWalkableCompsDense[i].callback == walkSwitchCBChangeLevel) {
            makeEntActive(gWalkableCompsDense[i].header.entId);
            break;
        }
    }
    if (!doesEventListenerExist(COMP_SLAPPABLE, E_MOVETASK_QUEUED, entId, eLDecrementSpinTaskSpeed, false))
        addEventListener(COMP_SLAPPABLE, E_MOVETASK_QUEUED, entId, eLDecrementSpinTaskSpeed, false);
    if (!doesEventListenerExist(COMP_SLAPPABLE, E_SLAPPABLE_STOPPED, entId, eLMakeGoalInactive, false))
        addEventListener(COMP_SLAPPABLE, E_SLAPPABLE_STOPPED, entId, eLMakeGoalInactive, false);
}

void eLMakeGoalInactive(EventListener* eL, int entId) {
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        if (gWalkableCompsDense[i].callback == walkSwitchCBChangeLevel) {
            makeEntInactive(gWalkableCompsDense[i].header.entId);
            break;
        }
    }
}

void eLDecrementSpinTaskSpeed(EventListener* eL, int entId) {
    TaskQueueComponent* switchTQ = getComponent(eL->listenerEntId, COMP_TASK_QUEUE);
    if (!switchTQ)
        return;
    if (!isTaskQueueEmpty(switchTQ)) {
        int timeRemaining = switchTQ->queue[switchTQ->head].data;
        int decrement = timeRemaining < 0 ? 1 : timeRemaining == INT32_MAX ? 0 : -1;
        switchTQ->queue[switchTQ->head].data += decrement;
        if (switchTQ->queue[switchTQ->head].data == 0) {
            switchTQ->head = (switchTQ->head + 1) % (sizeof(switchTQ->queue) / sizeof(Task));
            // PhysicsComponent* phys = getComponent(eL->listenerEntId, COMP_PHYSICS);
            // if (phys) {
            //     phys->angle = 0x2000;
            //     turnEnt(phys->header.entId, 0, 0);
            // }
            notify(eL->listenerEntId, COMP_SLAPPABLE, E_SLAPPABLE_STOPPED);
        }
    }
}

void taskSpinEndlessly(int entId, Task* task) {
    turnEnt(entId, task, task->data * 0x100);
}
