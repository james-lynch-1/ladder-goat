#include "component.h"

CellComponent* addComponentCell(int entId, int flags, int posX, int posY, int posZ) {
    CellComponent c = {{entId, flags}, {{posX}, {posY}, {posZ}}};
    return (CellComponent*)addComponentCustom(&c, COMP_CELL);
}

void removeComponentCell(int entId) {
    removeComponent(entId, COMP_CELL);
}
