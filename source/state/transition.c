#include "state.h"

int gTransitionStartingFrame = 0;

void isrVblankTransition() {
    gFrameCount++;
    gViewport.y -= 3;

    PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
    playerPhys->pos.y.HALF.HI += 2;
    scroll();
    updateObjs();
    REG_BG0VOFS = REG_BG1VOFS = gViewport.y;
    REG_BG0HOFS = REG_BG1HOFS = gViewport.x;

    copyObjAttrsToOAM();
    copyObjAffinesToOAM();
    logVal("vcount: ", REG_VCOUNT);
}

void enterTransition() {
    REG_DISPCNT |= DCNT_BG0 | DCNT_OBJ;
    gTransitionStartingFrame = gFrameCount;
    irq_add(II_VBLANK, isrVblankTransition);
}

void updateTransition() {
    bool hasScrolledOutOfView = gViewport.y < -SCREEN_HEIGHT - 64;
    static bool hasLoaded = false;
    if (hasScrolledOutOfView) {
        changeLevel(gNextLevel); // ZZZ stop this from changing viewport
        hasLoaded = true;
        gViewport.y = 8 * (gLevelData->xWidth + gLevelData->zWidth) + 64;
    }
    if (hasLoaded) {
        if (gViewport.y <= gLevelData->vp.y) {
            if (!hasComponent(gPlayerId, COMP_INPUT)) addComponentInput(gPlayerId, 0, handleInputPlayer);
            hasLoaded = false;
            setGameState(NORMAL);
            volatile PhysicsComponent* playerPhys = getComponent(gPlayerId, COMP_PHYSICS);
            playerPhys->pos.y.WORD = 0;
        }
    }

    // if (hasScrolledOutOfView) {
    //     changeLevel(gNextLevel);
    //     hasScrolledOutOfView = false;
    //     hasLoaded = true;
    // }
    // if (gViewport.y < -SCREEN_HEIGHT - 64) {
    //     gViewport.y = 256;
    //     hasScrolledOutOfView = true;
    // }



    // ObjComponent* obj = getComponent(gDeepestObjEntId[gIsLadderNWFacing], COMP_OBJ);
    // bool isLevelRestart = gNextLevel == gLevelData->levelId;
    // for (int i = 0; i < min(framesElapsed, numComps(COMP_OBJ)); i++) {
    //     int decrement = (isLevelRestart ? 1 : -1) * smoothStepLut[framesElapsed - i] + smoothStepLut[max(framesElapsed - 1, 0) - i];
    //     obj->yOffset += decrement;
    //     updateObj(obj->header.entId);
    //     if (obj->nextId[gIsLadderNWFacing] != INT16_MAX)
    //         obj = getComponent(obj->nextId[gIsLadderNWFacing], COMP_OBJ);
    // }
    // updateTaskQueues();
    // VBlankIntrWait();
    // if ((framesElapsed >= 30 &&
    //     (obj->attr0 & ATTR0_Y_MASK) > 160 && (obj->attr0 & ATTR0_Y_MASK) < 200) ||
    //     framesElapsed > 120) {
    //     if (gLevelData->levelId == 2 && !isLevelRestart) {
    //         setGameState(WIN);
    //         return;
    //     }
    //     changeLevel(gNextLevel);
    //     ObjComponent* playerObj = getComponent(gPlayerId, COMP_OBJ);
    //     ObjComponent* ladderObj = getComponent(gLadderId, COMP_OBJ);
    //     playerObj->yOffset = -4;
    //     ladderObj->yOffset = -22;
    //     updateObj(gPlayerId);
    //     updateObj(gLadderId);
    //     if (!hasComponent(gPlayerId, COMP_INPUT)) addComponentInput(gPlayerId, 0, handleInputPlayer);
    //     setGameState(NORMAL);
    // }
}

void exitTransition(enum GameState state) {
}
