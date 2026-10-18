#include "state.h"

void enterWin() {
    REG_DISPCNT &= ~DCNT_BG1;
    tte_set_pos(8, 8);
    tte_write("Thanks for playing!\n Press START to restart");
}

void updateWin() {
    key_poll();
    gOldViewport = gViewport;
    VBlankIntrWait();
    if ((gFrameCount & 1) == 0) {
        gScrollX++;
    }
    if ((gFrameCount & 3) == 0) {
        gScrollY++;
    }
    REG_BG0HOFS = gScrollX;
    REG_BG0VOFS = gScrollY;
    if (key_hit(KEY_START))
        reset();

    copyObjAttrsToOAM();
    copyObjAffinesToOAM();

    gFrameCount++;
}

void exitWin(enum GameState state) {
    tte_erase_screen();
}
