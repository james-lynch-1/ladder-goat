#include "player.h"
#include <assert.h>

int spawnPlayer(int playerX, int playerY, int playerZ, enum Direction playerDir, enum Direction keyDir) {
    int playerId = reserveEntSlot();
    gPlayerId = playerId;
    if (playerId == -1) {
        markEntToBeDeleted(playerId);
        return -1;
    }
    PhysicsComponent* phys = addComponentPhysics(
        playerId, PHYS_SOLID_FLAG, (16 * playerX) << 16,
        (16 * playerY) << 16,
        (16 * playerZ) << 16, 0, 0, 0, 2, playerDir * 0x4000);
    setClsnVal(phys->tilePos, (ComponentHeader*)phys);
    addComponentObj(
        playerId, OBJ_AFF_FLAG | OBJ_ZDEPTH_PRIO_1,
        ATTR0_AFF | ATTR0_TALL,
        ATTR1_SIZE_32x64 | ATTR1_AFF_ID(1),
        ATTR2_ID(fetchSprite(spritePlayerTiles, spritePlayerTilesLen)) | ATTR2_PALBANK(PAL_PLAYER),
        -4,
        COMP_PHYSICS
    );
    updateObj(playerId);

    addComponentRotation(playerId, 0);
    applyRotations(playerId);
    addComponentInput(playerId, 0, CBInputComp);
    addComponentTaskQueue(playerId, 0);
    addComponentGroup(playerId, COMP_PHYSICS, NULL, 0);

    return playerId;
}

void viewportMoveTowardTarget(int speedX, int speedY) {
    if (gViewport.targetX != INT32_MAX) {
        int sgnX = sgn3(gViewport.targetX - gViewport.x);
        gViewport.x += speedX * sgnX;
        int newSgnX = sgn3(gViewport.targetX - gViewport.x);
        if (ABS(sgnX - newSgnX) == 2)
            gViewport.x = gViewport.targetX;
    }
    if (gViewport.targetY != INT32_MAX) {
        int sgnY = sgn3(gViewport.targetY - gViewport.y);
        gViewport.y += speedY * sgnY;
        int newSgnY = sgn3(gViewport.targetY - gViewport.y);
        if (ABS(sgnY - newSgnY) == 2)
            gViewport.y = gViewport.targetY;
    }
}

void updateViewport() {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    GroupComponent* group = getComponent(gPlayerId, COMP_GROUP);
    ScreenPos playerScreenPos = getScreenPos(playerPhys->pos);
    PhysicsComponent* keyPhys = NULL;
    if (group)
        keyPhys = getComponent(group->memberIds[0], COMP_PHYSICS);
    int playerX = playerScreenPos.x;
    int playerY = playerScreenPos.y;
    if (keyPhys) {
        ScreenPos keyScreenPos = getScreenPosFromOldViewport(keyPhys->pos);
        int keyX = keyScreenPos.x;
        int keyY = keyScreenPos.y;
        if (playerX < 80 || keyX < 80)
            gViewport.x--;
        if (playerX > SCREEN_WIDTH - 80 || keyX > SCREEN_WIDTH - 80)
            gViewport.x++;
        if (playerY < 80 || keyY < 80)
            gViewport.y--;
        if (playerY > SCREEN_HEIGHT - 40 || keyY > SCREEN_HEIGHT - 40)
            gViewport.y++;
    }
    else {
        if (playerX < 80)
            gViewport.x--;
        if (playerX > SCREEN_WIDTH - 80)
            gViewport.x++;
        if (playerY < 80)
            gViewport.y--;
        if (playerY > SCREEN_HEIGHT - 40)
            gViewport.y++;
    }
    gViewport.x = clamp(gViewport.x, gViewport.xMin, gViewport.xMax + 1);
    gViewport.y = clamp(gViewport.y, gViewport.yMin, gViewport.yMax + 1);
}

void taskMoveDispatch(int entId, Task* task) {
    // PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    // if (checkCollisionMove(phys, task->data)) {
    //     task->timeRemaining = 0;
    //     return;
    // }
    // notify(entId, COMP_SLAPPABLE, E_MOVETASK_QUEUED);
    // int fwdBwdTribool = task->data;
    // int xTriBoolFromDirArr[4] = { fwdBwdTribool, 0, -fwdBwdTribool, 0 };
    // int xTriBool = xTriBoolFromDirArr[phys->tilePos.dir];
    // int zTriBool = xTriBoolFromDirArr[3 - phys->tilePos.dir];
    // phys->tilePos.x += xTriBool;
    // phys->tilePos.z += zTriBool;
    // addTaskGlobal(tGMoveDispatch, 1, entId, fwdBwdTribool);

    // // // now for the key(s)
    // GroupComponent* group = getComponent(entId, COMP_GROUP);
    // if (group && (group->header.flags == COMP_PHYSICS)) {
    //     for (int i = 0; i < group->numMembers; i++) {
    //         PhysicsComponent* memberPhys = getComponent(group->memberIds[i], COMP_PHYSICS);
    //         if (memberPhys && !checkCollisionCellMove(memberPhys, task->data)) {
    //             // addTaskToQueue(memberPhys->header.entId, TASK_MOVE, task->data);
    //             memberPhys->tilePos.x += xTriBool;
    //             memberPhys->tilePos.z += zTriBool;
    //             addTaskGlobal(tGMoveDispatch, 1, group->memberIds[i], fwdBwdTribool);
    //         }
    //     }
    // }
}

void getXZTribools(enum Direction dir, int fwdBwdTribool, int* xTribool, int* zTribool) {
    int xTriBoolFromDirArr[4] = { fwdBwdTribool, 0, -fwdBwdTribool, 0 };
    *xTribool = xTriBoolFromDirArr[dir];
    *zTribool = xTriBoolFromDirArr[3 - dir];
}

void taskMove(int entId, Task* task) {
    moveEnt(entId, task, task->data);
}

void moveEnt(int entId, Task* task, int dir) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    phys->vec.x.WORD = dir * lu_cos(phys->angle) << 4;
    phys->vec.z.WORD = dir * -lu_sin(phys->angle) << 4;
}

void taskTurnDispatch(int entId, Task* task) {
    bool canTurn = true;
    GroupComponent* group = getComponent(entId, COMP_GROUP);
    if (group && (group->header.flags == COMP_PHYSICS)) {
        for (int i = 0; i < group->numMembers; i++) {
            PhysicsComponent* memberPhys = getComponent(group->memberIds[i], COMP_PHYSICS);
            if (memberPhys && checkCollisionTurn(memberPhys, task->data)) {
                canTurn = false;
                break;
            }
        }
    }
    if (canTurn) {
        task->taskIndex = TASK_TURN;
        task->timeRemaining = gTaskTable[TASK_TURN].length;
        notify(entId, COMP_SLAPPABLE, E_MOVETASK_QUEUED);
        gTaskTable[task->taskIndex].fn(entId, task);
        for (int i = 0; i < group->numMembers; i++)
            addTaskToQueue(group->memberIds[i], TASK_TURN, task->data);
    }
    else
        task->timeRemaining = 0;
}

void taskTurn(int entId, Task* task) {
    turnEnt(entId, task, task->data * 0x4000 / 16);
}

void taskChangeLevel(int entId, Task* task) {
    ((InputComponent*)getComponent(gPlayerId, COMP_INPUT))->inputHandler = NULL;
    TaskQueueComponent* tQ = getComponent(gPlayerId, COMP_TASK_QUEUE);
    while (!isTaskQueueEmpty(tQ)) {
        tQ->head = (tQ->head + 1) % (sizeof(tQ->queue) / sizeof(Task));
    }
    static_assert(sizeof(Task) == 12); // don't want to divide by a non-power of 2

    gNextLevel = task->data;
    setGameState(TRANSITION);
}

void turnEnt(int entId, Task* task, int amount) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    RotationComponent* rot = getComponent(entId, COMP_ROTATION);
    phys->angle += amount;
    if (rot) setInitMatrix(phys, rot);
    applyRotations(entId);
}
