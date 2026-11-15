#include "ent.h"

int spawnEntKey(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void* data) {
    int entId = reserveEntSlot();
    int backEntId = reserveEntSlot();
    int frontEntId = reserveEntSlot();
    if (entId == -1 || backEntId == -1 || frontEntId == -1) {
        markEntToBeDeleted(entId);
        markEntToBeDeleted(frontEntId);
        markEntToBeDeleted(backEntId);
        return -1;
    }
    enum Direction dir = data0;
    PhysicsComponent* phys = addComponentPhysics(entId, PHYS_KEY_FLAG | PHYS_SOLID_FLAG, (16 * tileX) << 16, (16 * tileY) << 16, (16 * tileZ) << 16,
        0, 0, 0, 1, dir * 0x4000);
    phys->hitbox.fwd = phys->hitbox.bwd = 1;
    RotationComponent* rot = addComponentRotation(entId, 0);
    int xOffs = dir == SOUTHEAST ? 1 : dir == NORTHWEST ? -1 : 0;
    int zOffs = dir == SOUTHWEST ? 1 : dir == NORTHEAST ? -1 : 0;
    PhysicsComponent* frontPhys = addComponentPhysics(frontEntId, PHYS_FRONT_EDGE_FLAG | PHYS_SOLID_FLAG,
        phys->pos.x.WORD + ((xOffs * 16) << 16),
        phys->pos.y.WORD,
        phys->pos.z.WORD + ((zOffs * 16) << 16),
        0, 0, 0, 1, dir * 0x4000
    );
    PhysicsComponent* backPhys = addComponentPhysics(backEntId, PHYS_BACK_EDGE_FLAG | PHYS_SOLID_FLAG,
        phys->pos.x.WORD - ((xOffs * 16) << 16),
        phys->pos.y.WORD,
        phys->pos.z.WORD - ((zOffs * 16) << 16),
        0, 0, 0, 1, dir * 0x4000
    );
    addComponentObj(frontEntId, OBJ_AFF_FLAG | OBJ_ZDEPTH_PRIO_1,
        ATTR0_AFF | ATTR0_TALL,
        ATTR1_SIZE_32x64 | ATTR1_AFF_ID(1),
        ATTR2_ID(fetchSprite(spritePlayerTiles, spritePlayerTilesLen)) | ATTR2_PALBANK(PAL_KEY),
        -4,
        COMP_PHYSICS
    );
    addComponentObj(backEntId, OBJ_AFF_FLAG | OBJ_ZDEPTH_PRIO_1,
        ATTR0_AFF | ATTR0_TALL,
        ATTR1_SIZE_32x64 | ATTR1_AFF_ID(1),
        ATTR2_ID(fetchSprite(spritePlayerTiles, spritePlayerTilesLen)) | ATTR2_PALBANK(PAL_KEY),
        -4,
        COMP_PHYSICS
    );
    if (!phys ||
        !frontPhys ||
        !backPhys ||
        !addComponentGroup(entId, 0, NULL, 0) ||
        !addComponentMember(frontEntId, 0, entId) ||
        !addComponentMember(backEntId, 0, entId) ||
        !rot ||
        // !addComponentObj(
        //     entId, OBJ_AFF_DBL_FLAG | OBJ_ZDEPTH_PRIO_2,
        //     ATTR0_AFF_DBL | ATTR0_WIDE,
        //     ATTR1_SIZE_64x32 | ATTR1_AFF_ID(0),
        //     ATTR2_ID(fetchSprite(spriteHoriTiles, spriteHoriTilesLen)) | ATTR2_PALBANK(PAL_KEY),
        //     -22,
        //     COMP_PHYSICS) ||
        !addComponentObj(entId, OBJ_AFF_FLAG | OBJ_ZDEPTH_PRIO_1,
            ATTR0_AFF | ATTR0_TALL,
            ATTR1_SIZE_32x64 | ATTR1_AFF_ID(1),
            ATTR2_ID(fetchSprite(spritePlayerTiles, spritePlayerTilesLen)) | ATTR2_PALBANK(PAL_KEY),
            -4,
            COMP_PHYSICS
        ) ||
        !addComponentMember(entId, COMP_PHYSICS, gPlayerId) ||
        !addComponentTaskQueue(entId, 0)
        ) {
        markEntToBeDeleted(entId);
        markEntToBeDeleted(frontEntId);
        markEntToBeDeleted(backEntId);
        return -1;
    }
    setClsnVal(phys->tilePos, (ComponentHeader*)phys);
    setClsnVal(frontPhys->tilePos, (ComponentHeader*)frontPhys);
    setClsnVal(backPhys->tilePos, (ComponentHeader*)backPhys);
    setInitMatrix(phys, rot);
    applyRotations(entId);
    updateObj(entId);
    return entId;
}
