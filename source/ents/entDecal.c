#include "ent.h"

int spawnEntXAxisDecal(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void(*callback)) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, 0, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 1, 0) ||
        !addComponentObj(entId, OBJ_ZDEPTH_PRIO_1, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(spriteXAxisTiles, 512)) |
            ATTR2_PALBANK(PAL_ORANGE),
            0,
            COMP_PHYSICS)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}

int spawnEntZAxisDecal(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void(*callback)) {
    int entId = reserveEntSlot();
    if (entId == -1) return entId;
    gEntFlags[entId] |= entFlags;
    if (
        !addComponentPhysics(entId, 0, (tileX * 16) << 16, (tileY * 16) << 16, (tileZ * 16) << 16,
            0, 0, 0, 1, 0) ||
        !addComponentObj(entId, OBJ_ZDEPTH_PRIO_1, 0, ATTR1_SIZE_32x32,
            ATTR2_ID(fetchSprite(spriteZAxisTiles, 512)) |
            ATTR2_PALBANK(PAL_ORANGE),
            0,
            COMP_PHYSICS)) {
        markEntToBeDeleted(entId);
        return -1;
    }
    updateObj(entId);
    return entId;
}
