#include "component.h"

void walkableAddWeight(WalkableComponent* walk, int weight) {
    walk->weightToAdd = weight;
    walk->currentWeight += weight;
}

WalkableComponent* addComponentWalkable(int entId, int flags, int data0, int data1, int data2, int data3, void(*callback)) {
    WalkableComponent walk = { { entId, flags }, 0, 0,{ data0, data1, data2, data3 }, callback };
    return (WalkableComponent*)addComponentCustom(&walk, COMP_WALKABLE);
}

void removeComponentWalkable(int entId) {
    removeComponent(entId, COMP_WALKABLE);
}
