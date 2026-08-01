#ifndef STATE
#define STATE

#include "main.h"

void setGameState(enum GameState state);

// normal state functions

void enterNormal();

void updateNormal();

void exitNormal(enum GameState state);

// pause state functions

void enterPause();

void updatePause();

void exitPause(enum GameState state);

// title state functions

void enterTitle();

void updateTitle();

void exitTitle(enum GameState state);

// game over state functions

void enterGameOver();

void updateGameOver();

void exitGameOver(enum GameState state);

#endif
