#include "state.h"
#include <assert.h>

int gTransitionStartingFrame = 0;
bool hasScrolledOutOfView = false;
ScreenPos targetScreenPos;
bool playerPosYIsZero = false;
bool hasLoaded = false;

void updateViewportTransition(int speedX, int speedY) {
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

void isrVCountTransition() {
    key_poll();
    gOldViewport = gViewport;
    gFrameCount++;
    updateViewportTransition(1, 3);

    ObjComponent oldPlayerObj = *(ObjComponent*)getComponent(gPlayerId, COMP_OBJ);
    int y = (oldPlayerObj.attr0 & ATTR0_Y_MASK) >> ATTR0_Y_SHIFT;
    int x = (oldPlayerObj.attr1 & ATTR1_X_MASK) >> ATTR1_X_SHIFT;
    y += SGN3(targetScreenPos.y - y);
    x += SGN3(targetScreenPos.x - x);
    updateObjs();
    ObjComponent* playerObj = getComponent(gPlayerId, COMP_OBJ);
    *playerObj = oldPlayerObj;
    playerObj->attr0 = (playerObj->attr0 & ~ATTR0_Y_MASK) | ((y & 0b11111111) << ATTR0_Y_SHIFT);
    playerObj->attr1 = (playerObj->attr1 & ~ATTR1_X_MASK) | ((x & 0b111111111) << ATTR1_X_SHIFT);
}

void isrVBlankTransition() {
    scroll();
    if (!hasScrolledOutOfView || gViewport.y < 8 * (gLevelData->xWidth + gLevelData->zWidth) + 8) {
        copyObjAttrsToOAM();
        copyObjAffinesToOAM();
    }
    REG_BG0VOFS = gVpBackdrop.y--;
    REG_BG1VOFS = gViewport.y;
    REG_BG1HOFS = gViewport.x;
}

void enterTransition() {
    gTransitionStartingFrame = gFrameCount;
    gViewport.targetY = -SCREEN_HEIGHT - 64;
    REG_DISPSTAT |= DSTAT_VCT(140);
    irq_delete(II_VBLANK);
    irq_add(II_VBLANK, isrVBlankTransition);
    irq_add(II_VCOUNT, isrVCountTransition);

    hasScrolledOutOfView = false;
    Position newPos = { { (16 * gLevels[gNextLevel]->playerPos.x) << 16 },
                        { (16 * gLevels[gNextLevel]->playerPos.y) << 16 },
                        { (16 * gLevels[gNextLevel]->playerPos.z) << 16 } };
    int vpX = gLevels[gNextLevel]->vp.x;
    int vpY = gLevels[gNextLevel]->vp.y;
    targetScreenPos = getScreenPosFromViewportXY(newPos, gNextLevel, vpX, vpY);
    targetScreenPos.x -= 16;
    targetScreenPos.y -= 68;
    hasLoaded = false;
}

void updateTransition() {
    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    hasScrolledOutOfView = hasLoaded || gViewport.y <= -SCREEN_HEIGHT - 64;
    if (hasScrolledOutOfView && !hasLoaded) {
        memset32(&se_mem[SBB_MAP], 0, SBB_SIZE / 4);
        gViewport.x = gLevels[gNextLevel]->vp.canMove ? gViewport.x : gLevels[gNextLevel]->vp.x;
        gViewport.y = 8 * (gLevels[gNextLevel]->xWidth + gLevels[gNextLevel]->zWidth) + 96;
        gViewport.targetY = gLevels[gNextLevel]->vp.canMove ? gViewport.y : gLevels[gNextLevel]->vp.y;

        changeLevel(gNextLevel);
        hasLoaded = true;
    }
    if (hasLoaded && (playerPhys->pos.y.HALF.HI == gLevelData->playerPos.y * 16) &&
        ((gViewport.targetY == INT32_MAX) || gViewport.targetY == gViewport.y)) {
        ((InputComponent*)getComponent(gPlayerId, COMP_INPUT))->inputHandler = CBInputComp;
        hasLoaded = false;
        setGameState(NORMAL);
        RotationComponent* rot = getComponent(gPlayerId, COMP_ROTATION);
        setInitMatrix(playerPhys, rot);
    }
}

void exitTransition(enum GameState state) {
    REG_DISPSTAT &= ~DSTAT_VCT_MASK;
    irq_delete(II_VCOUNT);
}
