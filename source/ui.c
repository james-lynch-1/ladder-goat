#include "ui.h"

void updateUINormal() {
    tte_erase_line();
    memset32(&se_mem[UI_SBB], 0, SCREEN_WIDTH_T / 2);
    memset32((void*)(int)&se_mem[UI_SBB] + sizeof(SCR_ENTRY) * SBB_WIDTH_T * (SCREEN_HEIGHT_T - 1),
        0,
        SCREEN_WIDTH_T / 2);
}

void initialiseUi() {
    tte_init_se(
        BG_TEXT,
        BG_CBB(CBB_TEXT) | BG_SBB(TEXT_SBB) | BG_PRIO(PRIO_TEXT),
        0,
        CLR_WHITE,
        14,
        NULL,
        se_drawg_w8h8
    );
}
