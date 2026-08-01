#include "component.h"

CounterComponent* addComponentCounter(int entId, u16 flags, s16 curr, s16 max) {
    CounterComponent c = { {entId, flags}, curr, max, (SWord)0x10000 };
    CounterComponent* cAddr = (CounterComponent*)addComponentCustom(&c, COMP_COUNTER);
    return cAddr;
}

void removeComponentCounter(int entId) {
    removeComponent(entId, COMP_COUNTER);
}

CounterComponent* getCounterByFlags(int entId, u16 flags) {
    CounterComponent* counter = NULL;
    GroupComponent* group = getComponent(entId, COMP_GROUP);
    if (!group) return NULL;
    for (int i = 0; i < group->numMembers; i++) {
        counter = getComponent(group->memberIds[i], COMP_COUNTER);
        if (counter && (counter->header.flags & flags)) break;
    }
    return counter;
}

void incrementCounter(CounterComponent* counter, int amount) {
    counter->curr += amount;
    counter->curr = clamp(counter->curr, 0, counter->max + 1);
}
