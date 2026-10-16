#include "component.h"

// int walkableThresholdTribool

WalkableComponent* addComponentWalkable(int entId, int flags, int threshold, void(*callback)) {
    WalkableComponent walk = { { entId, flags }, 0, threshold, {}, callback };
    return (WalkableComponent*)addComponentCustom(&walk, COMP_WALKABLE);
}

void removeComponentWalkable(int entId) {
    removeComponent(entId, COMP_WALKABLE);
}
