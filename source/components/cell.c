#include "component.h"

CellComponent* addComponentCell(int entId, int flags, int tileX, int tileY, int tileZ) {
    CellComponent c = {{entId, flags}, {{(tileX * 16) << 16}, {(tileY * 16) << 16}, {(tileZ * 16) << 16}}};
    return (CellComponent*)addComponentCustom(&c, COMP_CELL);
}

void removeComponentCell(int entId) {
    removeComponent(entId, COMP_CELL);
}
