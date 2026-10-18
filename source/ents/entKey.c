#include "ent.h"

int spawnEntKey(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void* data) {
    int entId = reserveEntSlot();
    if (entId == -1) return -1;
    enum Direction dir = data0;
    PhysicsComponent* phys = addComponentPhysics(entId, PHYS_KEY_FLAG, (16 * tileX) << 16, (16 * tileY) << 16, (16 * tileZ) << 16,
        0, 0, 0, 1, dir * 0x2000);
    RotationComponent* rot = addComponentRotation(entId, 0);
    if (!phys ||
        !rot ||
        !addComponentObj(
            entId, OBJ_AFF_DBL_FLAG | OBJ_ZDEPTH_PRIO_2,
            ATTR0_AFF_DBL | ATTR0_WIDE,
            ATTR1_SIZE_64x32 | ATTR1_AFF_ID(0),
            ATTR2_ID(fetchSprite(spriteHoriTiles, spriteHoriTilesLen)) | ATTR2_PALBANK(PAL_KEY),
            -22,
            COMP_PHYSICS) ||
        !addComponentMember(entId, COMP_PHYSICS, gPlayerId) ||
        !addComponentTaskQueue(entId, 0)
        ) {
        markEntToBeDeleted(entId);
        return -1;
    }
    phys->hitbox.fwd = phys->hitbox.bwd = 1;
    setInitMatrix(phys, rot);
    applyRotations(entId);
    updateObj(entId);
    return entId;
}
