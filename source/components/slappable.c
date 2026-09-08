#include "component.h"

SlappableComponent* addComponentSlappable(int entId, int flags, int moveTimerLength, void(*callback)) {
    SlappableComponent slap = {{entId, flags}, moveTimerLength, callback};
    return (SlappableComponent*)addComponentCustom(&slap, COMP_SLAPPABLE);
}

void removeComponentSlappable(int entId) {
    removeComponent(entId, COMP_SLAPPABLE);
}
