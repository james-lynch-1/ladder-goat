#include "ent.h"

int spawnEntMover(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void(*callback)) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, PHYS_MOVER_FLAG | PHYS_SOLID_FLAG, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 2, 0) ||
        !addComponentObj(entId, 0, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(spriteMoverTiles, 512)) |
            ATTR2_PALBANK(PAL_ORANGE),
            8,
            COMP_PHYSICS) ||
        !addComponentTaskQueue(entId, 0)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}
