#include "state.h"

// normal state functions

void enterNormal() {
    loadBG(
        MAP_SBB,
        BG_MAP,
        MAP_PAL,
        checkerboardPal, checkerboardPalLen,
        checkerboardTiles, checkerboardTilesLen,
        checkerboardMap, checkerboardMapLen);
    // loadBG(
    //     MAP_SBB,
    //     BG_MAP,
    //     MAP_PAL,
    //     isometricPal, isometricPalLen,
    //     isometricTiles, isometricTilesLen,
    //     isometricMap, isometricMapLen
    // );
    REG_DISPCNT &= ~(DCNT_BG1);
    REG_DISPCNT |= DCNT_BG0 | DCNT_OBJ;
    tte_set_pos(SCREEN_WIDTH - TILE_WIDTH * 12 - 8, SCREEN_HEIGHT - 16);
    tte_write("START: RESET");
}

void updateNormal() {
    updateInputComps();
    updatePlayerStuff();
    updateTimers();
    // updateUINormal();
    updateTaskQueues();
    updatePhysics();
    deleteMarkedEnts();

    VBlankIntrWait();
}

void exitNormal(enum GameState state) {
    tte_erase_line();
}
