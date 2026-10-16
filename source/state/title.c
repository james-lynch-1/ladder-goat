#include "state.h"

// title state functions

void isrVblankTitle() {
    gFrameCount++;
}

void enterTitle() {
    REG_BG1VOFS = -80;
    REG_BG1HOFS = -8;
    REG_DISPCNT &= ~(DCNT_BG0 | DCNT_OBJ);
    loadBG(
        SBB_MAP, BG_MAP, PAL_MAP,
        ScratchLogoSmall1Pal, ScratchLogoSmall1PalLen,
        ScratchLogoSmall1Tiles, ScratchLogoSmall1TilesLen,
        ScratchLogoSmall1Map, ScratchLogoSmall1MapLen
    );
    tte_erase_screen();
    tte_set_pos(8, 16);
    tte_write("Brother Jauffre's\n Untitled Puzzle Game with\n Two Controllable Characters\n and Three Levels, the Last\n of Which is Fairly Difficult");
    tte_set_pos(8, 72);
    tte_write("PRESS START");
    irq_add(II_VBLANK, isrVblankTitle);
}

void updateTitle() {
    if (key_hit(KEY_START))
        setGameState(NORMAL);
}

void exitTitle(enum GameState state) {
    tte_erase_screen();

    spawnPlayer(2, 0, 2, 3, 0, 1, SOUTHEAST, SOUTHEAST);
    gNextLevel = 0;
    changeLevel(gNextLevel);
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
}
