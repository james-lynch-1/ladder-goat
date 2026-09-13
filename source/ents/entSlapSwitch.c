#include "ent.h"

int spawnEntSlapSwitch(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void(*callback)) {
    int entId = reserveEntSlot();
    int slapFlags = data0;
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
        !addComponentSlappable(entId, slapFlags, data0, data1, data2, data3, callback) ||
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
    int timeRemaining = slap->data[0] == INT32_MAX ? 6 : slap->data[0];
    if (!isTaskQueueEmpty(switchTQ))
        switchTQ->queue[switchTQ->head].data = turnDir * timeRemaining;
    else
        addTaskToQueue(entId, TASK_SPIN_ENDLESSLY, turnDir * timeRemaining);
}

void slapSwitchCBEnableGoal(int entId) {
    slapSwitchStartSpinning(entId);

    activateGoals();
    if (!doesEventListenerExist(COMP_SLAPPABLE, E_MOVETASK_QUEUED, entId, eLDecrementSpinTaskSpeed, false))
        addEventListener(COMP_SLAPPABLE, E_MOVETASK_QUEUED, entId, eLDecrementSpinTaskSpeed, false);
    if (!doesEventListenerExist(COMP_SLAPPABLE, E_SLAPPABLE_STOPPED, entId, eLMakeGoalInactive, false))
        addEventListener(COMP_SLAPPABLE, E_SLAPPABLE_STOPPED, entId, eLMakeGoalInactive, false);
}

void slapSwitchCBMoveMoversX(int entId) {
    TaskQueueComponent* playerTQ = getComponent(gPlayerId, COMP_TASK_QUEUE);
    bool isCW = (playerTQ->queue[playerTQ->head].taskIndex == TASK_TURN) &&
        (playerTQ->queue[playerTQ->head].data == 1) ?
        true : false;
    addTaskToQueue(entId, TASK_SPIN, isCW ? -1 : 1);
    for (int i = 0; i < numComps(COMP_PHYSICS); i++) {
        PhysicsComponent* phys = &gPhysCompsDense[i];
        if (phys->header.flags & PHYS_MOVER_FLAG)
            addTaskToQueue(phys->header.entId, TASK_MOVE_NESW, (SOUTHEAST + 2 * !isCW) % 4);
    }
}

void slapSwitchCBMoveMoversZ(int entId) {
    TaskQueueComponent* playerTQ = getComponent(gPlayerId, COMP_TASK_QUEUE);
    bool isCW = (playerTQ->queue[playerTQ->head].taskIndex == TASK_TURN) &&
        (playerTQ->queue[playerTQ->head].data == 1) ?
        true : false;
    addTaskToQueue(entId, TASK_SPIN, isCW ? -1 : 1);
    for (int i = 0; i < numComps(COMP_PHYSICS); i++) {
        PhysicsComponent* phys = &gPhysCompsDense[i];
        if (phys->header.flags & PHYS_MOVER_FLAG)
            addTaskToQueue(phys->header.entId, TASK_MOVE_NESW, (NORTHEAST + 2 * isCW) % 4);
    }
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
            notify(eL->listenerEntId, COMP_SLAPPABLE, E_SLAPPABLE_STOPPED);
        }
    }
}

void taskSpinEndlessly(int entId, Task* task) {
    int speed = 0x200;
    SlappableComponent* slap = getComponent(entId, COMP_SLAPPABLE);
    if (slap) speed = slap->data[1];
    turnEnt(entId, task, task->data * speed);
}

void taskSpinFinite(int entId, Task* task) {
    turnEnt(entId, task, task->data * task->timeRemaining * 0x180);
}
