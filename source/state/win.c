#include "state.h"

void enterWin() {
    loadBG(
        MAP_SBB, BG_MAP, MAP_PAL,
        isometricPal, isometricPalLen,
        isometricTiles, isometricTilesLen,
        isometricMap, isometricMapLen
    );
    tte_set_pos(8, 8);
    tte_write("Thanks for playing!\n Press START to restart");
}

void updateWin() {
    VBlankIntrWait();
    if ((gFrameCount & 3) == 0) {
        gScrollX += 2;
        gScrollY++;
        REG_BG0HOFS = gScrollX;
        REG_BG0VOFS = gScrollY;
    }
    if (key_hit(KEY_START)) {

        reset();
    }
}

void exitWin(enum GameState state) {
    tte_erase_screen();
}
