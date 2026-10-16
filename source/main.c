#include "main.h"

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
        // logVpPos();
    }

    return 0;
}
