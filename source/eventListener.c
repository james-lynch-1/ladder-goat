#include "eventListener.h"

bool doesEventListenerExist(enum ComponentType eventType, u32 eventFlags, int listenerEntId, void (*callback)(EventListener* eL, int entId), bool isRemovedAfterCallback) {
    EventListener eL = { eventFlags, listenerEntId, callback, isRemovedAfterCallback };
    for (int i = 0; i < gNumListenersPerType[eventType]; i++) {
        if ((gEventListeners[eventType][i].flags == eL.flags) &&
            (gEventListeners[eventType][i].callback == eL.callback) &&
            (gEventListeners[eventType][i].isRemovedAfterCallback == eL.isRemovedAfterCallback))
            return true;
    }
    return false;
}

bool addEventListener(enum ComponentType eventType, u32 eventFlags, int listenerEntId, void (*callback)(EventListener* eL, int entId), bool isRemovedAfterCallback) {
    if (gNumListenersPerType[eventType] == MAX_EVENT_LISTENERS_PER_TYPE) return false;
    EventListener eL = { eventFlags, listenerEntId, callback, isRemovedAfterCallback };
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
            eL.callback(&eL, entId);
            if (eL.isRemovedAfterCallback) {
                removeEventListener(compType, i);
                i--;
            }
        }
    }
}
