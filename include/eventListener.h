#ifndef EVENTLISTENER
#define EVENTLISTENER

#include "global.h"

#define E_NO_EVENT          0
#define E_PHYS_TOUCHED      0b1
#define E_PLAYER_DIED       0b10

bool addEventListener(enum ComponentType eventType, u32 flags, void(*callback)(int entId), bool removeAfterCallback);

void removeEventListener(enum ComponentType compType, int listenerIndex);

void notify(int entId, enum ComponentType compType, u32 flags);

#endif
