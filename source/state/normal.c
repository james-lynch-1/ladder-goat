#include "state.h"

// normal state functions

void enterNormal() {
    REG_DISPCNT |= DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;
    REG_BG1CNT = BG_PRIO(PRIO_UI) | BG_CBB(CBB_UI) | BG_SBB(UI_SBB) | BG_4BPP | BG_REG_32x32; // ui
}

void updateNormal() {
    updateInputComps();
    updatePlayerStuff();
    updatePhysics();
    updateRotComps();
    updateObjs();
    updateTimers();

    updateUINormal();

    updateTaskQueues();
    deleteMarkedEnts();

    VBlankIntrWait();
}

void exitNormal(enum GameState state) {
}
