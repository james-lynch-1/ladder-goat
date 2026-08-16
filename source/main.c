#include "main.h"

void copyObjAttrsToOAM() { // excluding affine stuff
    int idx = gDeepestObjEntId;
    for (int i = 0; i < numComps(COMP_OBJ); i++) {
        ObjComponent* o = getComponent(idx, COMP_OBJ);
        oam_mem[127 - i].attr0 = o->attr0;
        oam_mem[127 - i].attr1 = o->attr1;
        oam_mem[127 - i].attr2 = o->attr2;
        idx = o->nextId;
    }
    oam_init(oam_mem, 128 - numComps(COMP_OBJ));
}

void copyObjAffinesToOAM() {
    for (int i = 0; i < numComps(COMP_ROTATION); i++) {
        RotationComponent* rot = &gRotCompsDense[i];
        OBJ_AFFINE* oamAffPtr = &obj_aff_mem[rot->objAffIndex];
        oamAffPtr->pa = gObjAffBuffer[rot->objAffIndex].pa;
        oamAffPtr->pb = gObjAffBuffer[rot->objAffIndex].pb;
        oamAffPtr->pc = gObjAffBuffer[rot->objAffIndex].pc;
        oamAffPtr->pd = gObjAffBuffer[rot->objAffIndex].pd;
    }
}

int main() {
    initialiseGame();

    while (1) {
        key_poll();

        if (gGameState.justChanged) {
            gGameState.enterFunction();
            gGameState.justChanged = false;
        }
        gGameState.updateFunction();

        copyObjAttrsToOAM();
        copyObjAffinesToOAM();

        gFrameCount++;
    }

    return 0;
}
