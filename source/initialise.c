#include "initialise.h"

void initialiseObjPalettes() {
    memcpy32(&pal_obj_bank[PAL_PLAYER], spritePlayerPal, spritePlayerPalLen / sizeof(u32));
    memcpy32(&pal_obj_bank[PAL_LADDER], spriteHoriPal, spriteHoriPalLen / sizeof(u32));
    memcpy32(&pal_obj_bank[PAL_PURPLE], spriteCellPurplePal, spriteCellPurplePalLen / sizeof(u32));
    pal_obj_bank[PAL_PURPLE_REVERSED][0] = spriteCellPurplePal[0];
    pal_obj_bank[PAL_PURPLE_REVERSED][1] = spriteCellPurplePal[1];
    for (int i = 2; i < 16; i++)
        pal_obj_bank[PAL_PURPLE_REVERSED][i] = spriteCellPurplePal[15 - i + 2];
    memcpy32(&pal_obj_bank[PAL_ORANGE], spriteCellFenceNEPal, spriteCellFenceNEPalLen / sizeof(u32));
}

void initialiseGame() {
    oam_init(oam_mem, 128);
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    memset32(&gObjAllocArr, OBJ_SLOT_UNUSED, sizeof(gObjAllocArr) / 4);
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_PRIO(3) | BG_CBB(CBB_MAP) | BG_SBB(MAP_SBB) | BG_4BPP | BG_REG_32x32;
    REG_BG1CNT = BG_PRIO(3) | BG_CBB(CBB_UI) | BG_SBB(UI_SBB) | BG_4BPP | BG_REG_32x32;
    REG_BG2CNT = BG_PRIO(3) | BG_CBB(CBB_TEXT) | BG_SBB(TEXT_SBB) | BG_4BPP | BG_REG_32x32;

    memset32(&gNumListenersPerType, 0, sizeof(gNumListenersPerType) / 4);

    // components stuff
    initialiseComponentArrays();

    memset32(gSpriteAllocList, 0, sizeof(gSpriteAllocList) / 4);
    memset32(&gEntFlags, 0, sizeof(gEntFlags) / 4);
    memset16(gCompSetSparse, -1, NUM_COMP_TYPES * MAX_ENTS);
    memset32(gEntsToDelete, 0, sizeof(gEntsToDelete) / 4);

    initialiseObjPalettes();

    changeLevel(1);
    loadBG(
        MAP_SBB,
        isometricPal, isometricPalLen,
        isometricTiles, isometricTilesLen,
        isometricMap, isometricMapLen
    );

    gGameState.gameStateEnum = INT8_MAX;
    setGameState(NORMAL);
}
