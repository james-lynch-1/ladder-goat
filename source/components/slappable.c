#include "component.h"

SlappableComponent* addComponentSlappable(int entId, int flags, int data0, int data1, int data2, int data3, void(*callback)) {
    SlappableComponent slap = { { entId, flags },{ data0, data1, data2, data3 }, callback };
    return (SlappableComponent*)addComponentCustom(&slap, COMP_SLAPPABLE);
}

void removeComponentSlappable(int entId) {
    removeComponent(entId, COMP_SLAPPABLE);
}
