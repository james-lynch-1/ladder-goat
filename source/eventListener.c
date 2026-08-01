#include "eventListener.h"

bool addEventListener(enum ComponentType eventType, u32 flags, void (*callback)(int entId), bool removeAfterCallback) {
    if (gNumListenersPerType[eventType] == MAX_EVENT_LISTENERS_PER_TYPE) return false;
    EventListener eL = { flags, callback, removeAfterCallback };
    gEventListeners[eventType][gNumListenersPerType[eventType]++] = eL;
    return true;
}

void removeEventListener(enum ComponentType compType, int listenerIndex) {
    gEventListeners[compType][listenerIndex] = gEventListeners[compType][--gNumListenersPerType[compType]];
}

void notify(int entId, enum ComponentType compType, u32 flags) {
    for (int i = 0; i < gNumListenersPerType[compType]; i++) {
        EventListener eL = gEventListeners[compType][i];
        if (eL.flags & flags) {
            eL.callback(entId);
            if (eL.isRemovedAfterCallback) {
                removeEventListener(compType, i);
                i--;
            }
        }
    }
}
