#include "state.h"

// normal state functions

void enterNormal() {
    ADDR_BG_MAP |= BG_REG_32x32;
    drawBgTiles();
    REG_DISPCNT |= DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;
    tte_set_pos(SCREEN_WIDTH - TILE_WIDTH * 12 - 8, SCREEN_HEIGHT - 16);
    tte_write("START: RESET");
    irq_add(II_VBLANK, NULL);
}

void updateNormal() {
    updateInputComps();
    updatePlayerStuff();
    updateTimers();
    updateTaskQueues();
    updatePhysics();
    deleteMarkedEnts();
    if (gViewport.canMove) updateViewport();
    // if (gViewport.x != gOldViewport.x || gViewport.y != gOldViewport.y)
        updateObjs();
    VBlankIntrWait();
    REG_BG0HOFS = REG_BG1HOFS = gViewport.x;
    REG_BG0VOFS = REG_BG1VOFS = gViewport.y;

    scroll();

    copyObjAttrsToOAM();
    copyObjAffinesToOAM();

    gFrameCount++;
}

void exitNormal(enum GameState state) {
    tte_erase_line();
}
