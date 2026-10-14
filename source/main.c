#include "main.h"

void copyObjAttrsToOAM() { // excluding affine stuff
    memset32(oam_mem, ATTR0_HIDE, 2 * 128);
    int idx = gDeepestObjEntId[gIsLadderNWFacing];
    int i = 0;
    while (idx != INT16_MAX) {
        ObjComponent* o = getComponent(idx, COMP_OBJ);
        idx = o->nextId[gIsLadderNWFacing];
        if ((o->attr0 & ATTR0_MODE_MASK) == ATTR0_HIDE) {
            continue;
        }
        oam_mem[127 - i].attr0 = o->attr0;
        oam_mem[127 - i].attr1 = o->attr1;
        oam_mem[127 - i].attr2 = o->attr2;
        i++;
    }
}

void copyObjAffinesToOAM() {
    for (int i = 0; i < numComps(COMP_ROTATION); i++) {
        RotationComponent* rot = &gRotCompsDense[i];
        OBJ_AFFINE* oamAffPtr = &obj_aff_mem[gCompSetSparse[COMP_ROTATION][rot->header.entId]];
        oamAffPtr->pa = gObjAffBuffer[gCompSetSparse[COMP_ROTATION][rot->header.entId]].pa;
        oamAffPtr->pb = gObjAffBuffer[gCompSetSparse[COMP_ROTATION][rot->header.entId]].pb;
        oamAffPtr->pc = gObjAffBuffer[gCompSetSparse[COMP_ROTATION][rot->header.entId]].pc;
        oamAffPtr->pd = gObjAffBuffer[gCompSetSparse[COMP_ROTATION][rot->header.entId]].pd;
    }
}

int main() {
    initialiseGame();

    while (1) {
        gOldViewport = gViewport;
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
