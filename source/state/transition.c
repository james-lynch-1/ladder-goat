#include "state.h"

int gTransitionStartingFrame = 0;

void enterTransition() {
    REG_DISPCNT |= DCNT_BG0 | DCNT_OBJ;
    gTransitionStartingFrame = gFrameCount;
}

void updateTransition() {
    int framesElapsed = gFrameCount - gTransitionStartingFrame;
    ObjComponent* obj = getComponent(gDeepestObjEntId[gIsLadderNWFacing], COMP_OBJ);
    bool isLevelRestart = gNextLevel == gLevelData->levelId;
    for (int i = 0; i < min(framesElapsed, numComps(COMP_OBJ)); i++) {
        int decrement = (isLevelRestart ? 1 : -1) * smoothStepLut[framesElapsed - i] + smoothStepLut[max(framesElapsed - 1, 0) - i];
        obj->yOffset += decrement;
        updateObj(obj->header.entId);
        if (obj->nextId[gIsLadderNWFacing] != INT16_MAX)
            obj = getComponent(obj->nextId[gIsLadderNWFacing], COMP_OBJ);
    }
    updateTaskQueues();
    VBlankIntrWait();
    if ((framesElapsed >= 30 &&
        (obj->attr0 & ATTR0_Y_MASK) > 160 && (obj->attr0 & ATTR0_Y_MASK) < 200) ||
        framesElapsed > 120) {
        if (gLevelData->levelId == 2) {
            setGameState(WIN);
            return;
        }
        changeLevel(gNextLevel);
        ObjComponent* playerObj = getComponent(gPlayerId, COMP_OBJ);
        ObjComponent* ladderObj = getComponent(gLadderId, COMP_OBJ);
        playerObj->yOffset = 4;
        ladderObj->yOffset = -14;
        updateObj(gPlayerId);
        updateObj(gLadderId);
        addComponentInput(gPlayerId, 0, handleInputPlayer);
        setGameState(NORMAL);
    }
}

void exitTransition(enum GameState state) {
}
