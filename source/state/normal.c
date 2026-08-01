#include "state.h"

// normal state functions

void enterNormal() {
    REG_DISPCNT |= DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;
    REG_BG1CNT = BG_PRIO(PRIO_UI) | BG_CBB(CBB_UI) | BG_SBB(UI_SBB) | BG_4BPP | BG_REG_32x32; // ui
}

void updateNormal() {
    updateInputComps();

    VBlankIntrWait();
    updatePlayerStuff();
    updatePhysics();
    updateObjs();
    updateRotComps();
    updateTimers();

    updateUINormal();

    updateTaskQueues();
    deleteMarkedEnts();
}

void exitNormal(enum GameState state) {
}
