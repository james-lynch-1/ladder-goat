#include "state.h"

// pause state functions

void enterPause() {
    REG_BG1CNT &= ~BG_PRIO_MASK;
    REG_BG2CNT &= ~BG_PRIO_MASK;
}

void updatePause() {
    if (key_hit(KEY_START)) {
        setGameState(NORMAL);
        return;
    }
    if (key_hit(KEY_SELECT)) reset();
    tte_set_pos(SCREEN_WIDTH / 2 - 3 * TILE_WIDTH, SCREEN_HEIGHT - TILE_WIDTH * 3);
    tte_write("PAUSED\n    Press SELECT to retry.");
    VBlankIntrWait();
}

void exitPause(enum GameState state) {
    REG_BG1CNT |= BG_PRIO(PRIO_UI);
    REG_BG2CNT |= BG_PRIO(PRIO_TEXT);
    tte_erase_screen();
}
