#ifndef EVENTLISTENER
#define EVENTLISTENER

#include "global.h"

bool doesEventListenerExist(enum ComponentType eventType, u32 eventFlags, int listenerEntId, void(*callback)(EventListener* eL, int entId), bool isRemovedAfterCallback);

bool addEventListener(enum ComponentType eventType, u32 eventFlags, int listenerEntId, void(*callback)(EventListener* eL, int entId), bool isRemovedAfterCallback);

void removeEventListener(enum ComponentType compType, int listenerIndex);

void notify(int entId, enum ComponentType compType, u32 flags);

#endif
