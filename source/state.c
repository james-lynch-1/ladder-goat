#include "state.h"

void setGameState(enum GameState state) {
    if (gGameState.gameStateEnum < NUM_GAME_STATES) gGameState.exitFunction(gGameState.gameStateEnum);
    gGameState.gameStateEnum = state;
    gGameState.justChanged = true;
    switch (state) {
        case NORMAL:
            gGameState.enterFunction = enterNormal;
            gGameState.updateFunction = updateNormal;
            gGameState.exitFunction = exitNormal;
            break;
        case PAUSE:
            gGameState.enterFunction = enterPause;
            gGameState.updateFunction = updatePause;
            gGameState.exitFunction = exitPause;
            break;
        case TITLE:
            gGameState.enterFunction = enterTitle;
            gGameState.updateFunction = updateTitle;
            gGameState.exitFunction = exitTitle;
            break;
        case GAMEOVER:
            gGameState.enterFunction = enterGameOver;
            gGameState.updateFunction = updateGameOver;
            gGameState.exitFunction = exitGameOver;
            break;
        default:
            break;
    }
}
