#include "main.h"

int main() {
    initialiseGame();

    while (1) {
        key_poll();
        
        if (gGameState.justChanged) {
            gGameState.enterFunction();
            gGameState.justChanged = false;
        }
        gGameState.updateFunction();
        
        oam_copy(oam_mem, gObjBuffer, 128);
        obj_aff_copy(obj_aff_mem, gObjAffBuffer, 1);
        gFrameCount++;
    }

    return 0;
}
