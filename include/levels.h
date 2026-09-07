#ifndef LEVELS
#define LEVELS

#include "global.h"

extern const LevelData level0;
extern const LevelData level1;

extern const LevelData* gLevels[2];

void changeLevel(int levelId);

#endif
