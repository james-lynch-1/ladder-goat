#include "player.h"
#include <assert.h>

int spawnPlayer(int playerX, int playerY, int playerZ, enum Direction playerDir, enum Direction keyDir) {
    int playerId = reserveEntSlot();
    gPlayerId = playerId;
    if (playerId == -1) {
        markEntToBeDeleted(playerId);
        return -1;
    }
    addComponentPhysics(
        playerId, PHYS_SOLID_FLAG, (16 * playerX) << 16,
        (16 * playerY) << 16,
        (16 * playerZ) << 16, 0, 0, 0, 2, playerDir * 0x4000);
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

void updateViewport() {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    ScreenPos playerScreenPos = getScreenPos(playerPhys->pos);
    int playerX = playerScreenPos.x % 256;
    int playerY = playerScreenPos.y % 256;
    if (playerX < 48)
        // gViewport.x -= (48 - playerX);
        gViewport.x--;
    if (playerX > SCREEN_WIDTH - 48)
        gViewport.x++;
        // gViewport.x += (playerX - (SCREEN_WIDTH - 48));
    if (playerY < 64)
        // gViewport.y -= (64 - playerY);
        gViewport.y--;
    if (playerY > SCREEN_HEIGHT - 16)
        // gViewport.y += (playerY - (SCREEN_HEIGHT - 16));
        gViewport.y++;
}

void taskMoveDispatch(int entId, Task* task) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if (checkCollisionMove(phys, task->data)) {
        task->timeRemaining = 0;
        return;
    }
    notify(entId, COMP_SLAPPABLE, E_MOVETASK_QUEUED);
    task->taskIndex = TASK_MOVE;
    task->timeRemaining = gTaskTable[TASK_MOVE].length;
    gTaskTable[task->taskIndex].fn(entId, task);

    // now for the key(s)
    GroupComponent* group = getComponent(entId, COMP_GROUP);
    if (group && (group->header.flags == COMP_PHYSICS)) {
        for (int i = 0; i < group->numMembers; i++) {
            PhysicsComponent* memberPhys = getComponent(group->memberIds[i], COMP_PHYSICS);
            if (memberPhys && !checkCollisionMove(memberPhys, task->data))
                addTaskToQueue(memberPhys->header.entId, TASK_MOVE, task->data);
        }
    }
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

void updatePlayerStuff() {
    GroupComponent* group = getComponent(gPlayerId, COMP_GROUP);
    if (group && (group->numMembers > 0)) {
        PhysicsComponent* keyPhys = getComponent(group->memberIds[0], COMP_PHYSICS);
        if (keyPhys)
            gIsKeyNWFacing = keyPhys->angle >= 0xE000 || keyPhys->angle <= 0x1FFF ||
            (keyPhys->angle >= 0x6000 && keyPhys->angle <= 0x9FFF);
    }
}
