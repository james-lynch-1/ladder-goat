#ifndef ENEMY
#define ENEMY

#include "component.h"
#include "graphics.h"
#include "entity.h"

int spawnEnemyWeak(int x, int y);

int spawnEnemy(int x, int y);

void splitEnemies();

void spawnEnemyConditional();

void spawnEnemyCentred();

#endif
