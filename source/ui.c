#include "ui.h"

void updateUINormal() {
    tte_erase_line();
    memset32(&se_mem[SBB_UI], 0, SCREEN_WIDTH_T / 2);
    memset32((void*)(int)&se_mem[SBB_UI] + sizeof(SCR_ENTRY) * SBB_WIDTH_T * (SCREEN_HEIGHT_T - 1),
        0,
        SCREEN_WIDTH_T / 2);
}

void initialiseUi() {
    tte_init_se(
        BG_UI,
        BG_CBB(CBB_UI) | BG_SBB(SBB_UI) | BG_PRIO(PRIO_UI),
        0,
        CLR_ORANGE,
        14,
        NULL,
        se_drawg_w8h8
    );
}
