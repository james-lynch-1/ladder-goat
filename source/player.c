#include "player.h"

int spawnPlayer(int ladderX, int ladderY, int ladderZ, int playerX, int playerY, int playerZ) {
    int entId = reserveEntSlot();
    int ladderId = reserveEntSlot();
    int playerId = reserveEntSlot();
    gLadderId = ladderId;
    gPlayerId = playerId;
    if (entId == -1 || ladderId == -1 || playerId == -1) {
        markEntToBeDeleted(entId);
        markEntToBeDeleted(ladderId);
        markEntToBeDeleted(playerId);
        return -1;
    }

    addComponentGroup(entId, 0, NULL, NULL, 0, ENT_PLAYER);

    // ladder stuff
    PhysicsComponent* ladderPhys = addComponentPhysics(
        ladderId, 0, (16 * ladderX) << 16,
        (16 * ladderY) << 16,
        (16 * ladderZ) << 16, 0, 0, 0, 0);
    ladderPhys->hitbox.fwd = ladderPhys->hitbox.bwd = 1;
    addComponentObj(
        ladderId, OBJ_AFF_DBL_FLAG | 1,
        ATTR0_AFF_DBL | ATTR0_WIDE,
        ATTR1_SIZE_64x32 | ATTR1_AFF_ID(0),
        ATTR2_ID(fetchSprite(spriteHoriTiles, spriteHoriTilesLen)) | ATTR2_PALBANK(PAL_LADDER),
        -16,
        COMP_PHYSICS
    );
    updateObj(ladderId);

    addComponentRotation(ladderId, 0);
    applyRotations(ladderId);
    addComponentInput(ladderId, 0, handleInputLadder);
    addComponentMember(ladderId, 0, entId);

    // player stuff
    addComponentPhysics(
        playerId, 0, (16 * playerX) << 16,
        (16 * playerY) << 16,
        (16 * playerZ) << 16, 0, 0, 0, 0);
    addComponentObj(
        playerId, OBJ_AFF_DBL_FLAG,
        ATTR0_AFF_DBL | ATTR0_TALL,
        ATTR1_SIZE_32x64 | ATTR1_AFF_ID(1),
        ATTR2_ID(fetchSprite(spritePlayerTiles, spritePlayerTilesLen)) | ATTR2_PALBANK(PAL_PLAYER),
        -28,
        COMP_PHYSICS
    );
    updateObj(playerId);
    
    addComponentRotation(playerId, 0);
    applyRotations(playerId);
    addComponentInput(playerId, 0, handleInputPlayer);
    addComponentMember(playerId, 0, entId);
    addComponentTaskQueue(playerId, 0);

    return entId;
}

void taskMove(int entId, Task* task) {
    int dir = task->data == 1 ? 1 : -1;
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    PhysicsComponent* ladderPhys = getComponent(gLadderId, COMP_PHYSICS);
    bool canMovePlayer = checkCollisionMove(playerPhys, dir) == 0;
    bool canMoveLadder = checkCollisionMove(ladderPhys, dir) == 0;
    const int taskIndexes[4] = { 0, TASK_MOVE_PLAYER, 0, TASK_MOVE_PLAYER_AND_LADDER };
    int taskIndexesIndex = (canMoveLadder << 1) | canMovePlayer;
    if (taskIndexesIndex == 0) return;
    if (taskIndexes[taskIndexesIndex] == 0) {
        task->timeRemaining = 0;
        return;
    }

    task->taskIndex = taskIndexes[taskIndexesIndex];
    task->timeRemaining = gTaskTable[task->taskIndex].length;
    gTaskTable[task->taskIndex].fn(entId, task);
}

void taskMovePlayerAndLadder(int entId, Task* task) {
    int dir = task->data == 1 ? 1 : -1;
    moveEnt(gLadderId, task, dir);
    moveEnt(gPlayerId, task, dir);
}

void taskMovePlayer(int entId, Task* task) {
    int dir = task->data == 1 ? 1 : -1;
    moveEnt(gPlayerId, task, dir);
}

void taskMoveLadder(int entId, Task* task) {
    int dir = task->data == 1 ? 1 : -1;
    moveEnt(gLadderId, task, dir);
}

void moveEnt(int entId, Task* task, int dir) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    phys->vec.x.WORD = dir * lu_cos(phys->angle) << 4;
    phys->vec.z.WORD = dir * -lu_sin(phys->angle) << 4;
}

void taskTurn(int entId, Task* task) {
    int dir = task->data == 1 ? 1 : -1;
    PhysicsComponent* ladderPhys = getComponent(gLadderId, COMP_PHYSICS);
    if (task->timeRemaining == gTaskTable[task->taskIndex].length &&
        checkCollisionTurn(ladderPhys, dir)) {
        task->timeRemaining = 1;
        return;
    }
    turnEnt(gLadderId, task, dir);
    turnEnt(gPlayerId, task, dir);
}

void turnEnt(int entId, Task* task, int dir) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    RotationComponent* rot = getComponent(entId, COMP_ROTATION);
    phys->angle += dir * 0x4000 / 16;
    int visAngle = phys->angle;
    if ((visAngle & (UINT16_MAX / 2)) == 0x6000) // avoid having the affine matrix be 0
        visAngle += 128 * dir;
    Matrix3D mtx = { {lu_cos(visAngle) << 4}, {0}, {-lu_sin(visAngle) << 4},
                     {0},{0x10000}, {0},
                     {lu_sin(visAngle) << 4}, {0}, {lu_cos(visAngle) << 4} };
    rot->mtx = mtx;
    applyRotations(entId);
}

void updatePlayerStuff() {
    PhysicsComponent* ladderPhys = getComponent(gLadderId, COMP_PHYSICS);
    gIsLadderNWFacing = ladderPhys->angle >= 0xE000 || ladderPhys->angle <= 0x1FFF ||
        (ladderPhys->angle >= 0x6000 && ladderPhys->angle <= 0x9FFF);
}
