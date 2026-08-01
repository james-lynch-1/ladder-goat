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
    addComponentPhysics(ladderId, 0, ladderX << 16, ladderY << 16, ladderZ << 16, NULL, 0, 0, 0, 0);
    ObjComponent* ladderObj = addComponentObj(ladderId, OBJ_AFF_DBL_FLAG, COMP_PHYSICS);
    memcpy32(&pal_obj_bank[0], spriteLadderPal, spriteLadderPalLen / sizeof(u32));
    obj_set_attr(getObj(ladderObj),
        ATTR0_AFF_DBL | ATTR0_WIDE,
        ATTR1_SIZE_64x32 | ATTR1_AFF_ID(0),
        ATTR2_ID(fetchSprite(spriteLadderTiles, spriteLadderTilesLen)));
    addComponentObjAff(ladderId, 0, true);
    obj_aff_identity(&gObjAffBuffer[0]);
    addComponentRotation(ladderId, 0);
    addComponentInput(ladderId, 0, handleInputLadder);
    addComponentMember(ladderId, 0, entId);

    // player stuff
    ObjComponent* playerObj = addComponentObj(playerId, 0, COMP_PHYSICS);
    memcpy32(&pal_obj_bank[1], spritePlayerPal, spritePlayerPalLen / sizeof(u32));
    obj_set_attr(getObj(playerObj),
        ATTR0_REG | ATTR0_TALL,
        ATTR1_SIZE_16x32,
        ATTR2_ID(fetchSprite(spritePlayerTiles, spritePlayerTilesLen)) | ATTR2_PALBANK(1));
    addComponentPhysics(playerId, 0, playerX << 16, playerY << 16, playerZ << 16, NULL, 0, 0, 0, 0);
    addComponentInput(playerId, 0, handleInputPlayer);
    addComponentMember(playerId, 0, entId);
    addComponentTaskQueue(playerId, 0);

    return entId;
}

void taskMoveForward(Task* task) {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    PhysicsComponent* ladderPhys = getComponent(gLadderId, COMP_PHYSICS);
    playerPhys->vec.x.WORD = ladderPhys->vec.x.WORD = lu_cos(playerPhys->angle) << 4;
    playerPhys->vec.z.WORD = ladderPhys->vec.z.WORD = lu_sin(playerPhys->angle) << 4;
}

void taskMoveBackward(Task* task) {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    PhysicsComponent* ladderPhys = getComponent(gLadderId, COMP_PHYSICS);
    playerPhys->vec.x.WORD = ladderPhys->vec.x.WORD = -lu_cos(playerPhys->angle) << 4;
    playerPhys->vec.z.WORD = ladderPhys->vec.z.WORD = -lu_sin(playerPhys->angle) << 4;
}

void taskTurnLeft(Task* task) {
    turnPlayer(task, -1);
}

void taskTurnRight(Task* task) {
    turnPlayer(task, 1);
}

void turnPlayer(Task* task, int dir) {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    PhysicsComponent* ladderPhys = getComponent(gLadderId, COMP_PHYSICS);
    RotationComponent* rot = getComponent(gLadderId, COMP_ROTATION);
    playerPhys->angle += dir * 0x4000 / 16;
    ladderPhys->angle += dir * 0x4000 / 16;
    Matrix3D mtx = { {lu_cos(playerPhys->angle) << 4}, {0}, {lu_sin(playerPhys->angle) << 4},
                     {0},{0x10000}, {0},
                     {-lu_sin(playerPhys->angle) << 4}, {0}, {lu_cos(playerPhys->angle) << 4} };
    rot->mtx = mtx;
}

void updatePlayerStuff() {
    // input component update (elsewhere) moves the phys comp
    // 
}

void handlePlayerDied() {

}
