#include "state.h"

// title state functions

void enterTitle() {
    REG_BG1VOFS = -80;
    REG_BG1HOFS = -8;
    REG_DISPCNT &= ~(DCNT_BG0 | DCNT_OBJ);
    loadBG(UI_SBB, BG_UI, UI_PAL, ScratchLogoSmall1Pal, ScratchLogoSmall1PalLen,
        ScratchLogoSmall1Tiles, ScratchLogoSmall1TilesLen,
        ScratchLogoSmall1Map, ScratchLogoSmall1MapLen);
    tte_erase_screen();
    tte_set_pos(8, 16);
    tte_write("Brother Jauffre's\n Untitled Puzzle Game with\n Two Controllable Characters\n and Three Levels, the Last\n of Which is Fairly Difficult");
    tte_set_pos(8, 72);
    tte_write("PRESS START");
}

void updateTitle() {
    if (key_hit(KEY_START))
        setGameState(NORMAL);
}

void exitTitle(enum GameState state) {
    tte_erase_screen();
}
