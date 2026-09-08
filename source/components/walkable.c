#include "component.h"

void walkableAddWeight(WalkableComponent* walk, int weight) {
    walk->weightToAdd = weight;
    walk->currentWeight += weight;
}

WalkableComponent* addComponentWalkable(int entId, int flags, int moveTimerLength, void(*callback)) {
    WalkableComponent walk = { {entId, flags}, 0, 0, moveTimerLength, callback };
    return (WalkableComponent*)addComponentCustom(&walk, COMP_WALKABLE);
}

void removeComponentWalkable(int entId) {
    removeComponent(entId, COMP_WALKABLE);
}
