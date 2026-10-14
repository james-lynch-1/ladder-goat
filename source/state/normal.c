#include "state.h"

// normal state functions

void enterNormal() {
    ADDR_BG_BACKDROP |= BG_REG_64x64;
    ADDR_BG_MAP |= BG_REG_32x32;
    drawBgTiles();
    loadBG(
        SBB_BACKDROP,
        BG_BACKDROP,
        PAL_BACKDROP,
        towerPal, towerPalLen,
        towerTiles, towerTilesLen,
        towerMap, towerMapLen
    );
    memcpy16(&pal_bg_bank[PAL_MAP], checkerboard_fgPal, checkerboard_fgPalLen / sizeof(u16));
    memcpy32(&tile_mem[BG_MAP], checkerboard_fgTiles, checkerboard_fgTilesLen / sizeof(u32));
    // loadBG(
    //     SBB_MAP,
    //     BG_MAP,
    //     PAL_MAP,
    //     checkerboardPal, checkerboardPalLen,
    //     checkerboardTiles, checkerboardTilesLen,
    //     checkerboardMap, checkerboardMapLen);
    REG_DISPCNT |= DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;
    tte_set_pos(SCREEN_WIDTH - TILE_WIDTH * 12 - 8, SCREEN_HEIGHT - 16);
    tte_write("START: RESET");
}

void logVpPos() {
    tte_erase_line();
    tte_set_pos(SCREEN_WIDTH - TILE_WIDTH * 12 - 32, SCREEN_HEIGHT - 16);
    tte_write("x:");
    char str[8];
    sprintf(str, "%d", gViewport.x);
    str[7] = '\0';
    tte_write(str);
    tte_set_pos(SCREEN_WIDTH - TILE_WIDTH * 12 + 32, SCREEN_HEIGHT - 16);
    tte_write("y:");
    memset32(str, 0, 1);
    sprintf(str, "%d", gViewport.y);
    str[7] = '\0';
    tte_write(str);
}

void updateNormal() {
    updateInputComps();
    updatePlayerStuff();
    updateTimers();
    updateTaskQueues();
    updatePhysics();
    deleteMarkedEnts();
    if (gViewport.canMove) scroll();
    if (gViewport.x != gOldViewport.x || gViewport.y != gOldViewport.y)
        updateObjs();
    VBlankIntrWait();
    REG_BG0HOFS = REG_BG1HOFS = gViewport.x;
    REG_BG0VOFS = REG_BG1VOFS = gViewport.y;

    int scrollYDiff = (gViewport.y & ~0b111) / 8 - (gOldViewport.y & ~0b111) / 8;
    if (scrollYDiff) {
        drawBgTileRow(scrollYDiff > 0);
    }

    int scrollXDiff = (gViewport.x & ~0b1111) / 16 - (gOldViewport.x & ~0b1111) / 16;
    if (scrollXDiff) {
        drawBgTileCol(scrollXDiff > 0);
    }
}

void exitNormal(enum GameState state) {
    tte_erase_line();
}
