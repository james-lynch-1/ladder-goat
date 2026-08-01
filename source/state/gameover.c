#include "state.h"

void allowRetry() {
    tte_write(" Press START to retry.\n");
}

void enterGameOver() {
    REG_BG1CNT &= ~BG_PRIO_MASK;
    REG_BG2CNT &= ~BG_PRIO_MASK;
    tte_set_color(0, CLR_RED);
    tte_set_pos(0, 24);
    tte_write(" GAME OVER\n");
    for (int i = 0; i < numComps(COMP_TIMER); i++)
        removeComponent(gTimerCompsDense[i].header.entId, COMP_TIMER);
    addComponentTimer(reserveEntSlot(), TIMER_DELETE_ENT, 60, allowRetry);
}

void updateGameOver() {
    handleInputGameover(gPlayerId);
    updateTimers();
    deleteMarkedEnts();
    VBlankIntrWait();
}

void exitGameOver(enum GameState state) {
    // REG_BG1CNT |= BG_PRIO(PRIO_UI);
    // REG_BG2CNT |= BG_PRIO(PRIO_TEXT);
    // tte_erase_screen();
    reset();
}
