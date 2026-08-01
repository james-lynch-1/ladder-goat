#include "state.h"

// title state functions

void enterTitle() {
    REG_DISPCNT &= ~(DCNT_BG1 | DCNT_OBJ);
    tte_erase_screen();
    tte_set_pos(8, 8);
    tte_write("PRESS START");
}

void updateTitle() {
    if (key_hit(KEY_START)) {
        setGameState(NORMAL);
    }
}

void exitTitle(enum GameState state) {
    tte_erase_screen();
}
