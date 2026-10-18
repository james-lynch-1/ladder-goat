#include "state.h"

// normal state functions

void enterNormal() {
    ADDR_BG_MAP |= BG_REG_32x32;
    updateObjs();

    PhysicsComponent* phys = getComponent(gPlayerId, COMP_PHYSICS);
    RotationComponent* rot = getComponent(gPlayerId, COMP_ROTATION);
    if (phys && rot) {
        setInitMatrix(phys, rot);
        applyRotations(gPlayerId);
    }
    
    REG_DISPCNT |= DCNT_BG0 | DCNT_BG1 | DCNT_OBJ;
    tte_erase_line();
    tte_set_pos(SCREEN_WIDTH - TILE_WIDTH * 12 - 8, SCREEN_HEIGHT - 16);
    tte_write("START: RESET");
    irq_delete(II_VBLANK);
    irq_add(II_VBLANK, NULL);
    REG_DISPSTAT &= ~DSTAT_VCT_MASK;
    irq_delete(II_VCOUNT);
}

void updateNormal() {
    gOldViewport = gViewport;
    key_poll();

    updateTaskQueues();
    handleInput();
    updateInputComps();
    updatePlayerStuff();
    updateTimers();
    updatePhysics();
    deleteMarkedEnts();
    if (gViewport.canMove)
        updateViewport();
    if (gViewport.x != gOldViewport.x || gViewport.y != gOldViewport.y)
        updateObjs();
    VBlankIntrWait();
    REG_BG1HOFS = gViewport.x;
    REG_BG1VOFS = gViewport.y;
    REG_BG0HOFS = gVpBackdrop.x;
    REG_BG0VOFS = gVpBackdrop.y;

    scroll();

    copyObjAttrsToOAM();
    copyObjAffinesToOAM();

    gFrameCount++;
}

void exitNormal(enum GameState state) {
    tte_erase_line();
}
