#include "initialise.h"

void initialiseGame() {
    irq_init(NULL);
    irq_add(II_VBLANK, NULL);
    // oam_init(gObjBuffer, 128);
    memset32(&gObjAllocArr, OBJ_SLOT_UNUSED, sizeof(gObjAllocArr) / 4);
    REG_DISPCNT = DCNT_MODE0 | DCNT_BG0 | DCNT_BG1 | DCNT_BG2 | DCNT_OBJ | DCNT_OBJ_1D;
    REG_BG0CNT = BG_PRIO(3) | BG_CBB(CBB_MAP) | BG_SBB(MAP_SBB) | BG_4BPP | BG_REG_32x32;
    REG_BG1CNT = BG_PRIO(3) | BG_CBB(CBB_UI) | BG_SBB(UI_SBB) | BG_4BPP | BG_REG_32x32;
    REG_BG2CNT = BG_PRIO(3) | BG_CBB(CBB_TEXT) | BG_SBB(TEXT_SBB) | BG_4BPP | BG_REG_32x32;

    memset32(&gNumListenersPerType, 0, sizeof(gNumListenersPerType) / 4);

    // components stuff
    initialiseComponentArrays();
    // initialiseArchetypeArrays();

    memset32(gSpriteAllocList, 0, sizeof(gSpriteAllocList) / 4);
    memset32(&gEntFlags, 0, sizeof(gEntFlags) / 4);
    memset16(gCompSetSparse, -1, NUM_COMP_TYPES * MAX_ENTS);
    memset32(gEntsToDelete, 0, sizeof(gEntsToDelete) / 4);

    loadBG(MAP_SBB, isometricPal, isometricPalLen, isometricTiles, isometricTilesLen, isometricMap, isometricMapLen);

    // for (int i = 0; i < 16; i++) {
    //     memcpy32(&pal_obj_bank[i], (*(encounter->paletteSet))[i], 8);
    // }

    // spawnPlayer(8 * MT_WIDTH, MT_WIDTH / 2, 0, 8 * MT_WIDTH, PLAYER_HEIGHT / 2, 0);
    spawnPlayer(7, 0, 7, 7, 0, 7);
    initSpriteCells();

    memcpy32(&pal_obj_bank[2], spriteCellPurplePal, spriteCellPurplePalLen / sizeof(u32));

    gGameState.gameStateEnum = INT8_MAX;
    setGameState(NORMAL);
}
