#ifndef STATE
#define STATE

#include "main.h"

void setGameState(enum GameState state);

// normal state functions

void enterNormal();

void updateNormal();

void exitNormal(enum GameState state);

// transition state functions

void enterTransition();

void updateTransition();

void exitTransition(enum GameState state);

// title state functions

void enterTitle();

void updateTitle();

void exitTitle(enum GameState state);

// pause state functions

void enterPause();

void updatePause();

void exitPause(enum GameState state);

// win state functions

void enterWin();

void updateWin();

void exitWin(enum GameState state);

#endif
