#include "ent.h"

int spawnEntGoal(int tileX, int tileY, int tileZ, int entFlags, int data0, int data1, int data2, int data3, void(*callback)) {
    int entId = spawnEntWalkSwitch(tileX, tileY, tileZ, entFlags, data0, data1, data2, data3, callback);
    if (entId == -1) return entId;
    addComponentTaskQueue(entId, 0);
    addComponentRotation(entId, 0);
    applyRotations(entId);

    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    obj->yOffset = 2;
    stopUsingSprite(obj->attr2 & ATTR2_ID_MASK);
    obj->attr2 &= ~(ATTR2_ID_MASK | ATTR2_PALBANK_MASK);
    obj->attr2 |= fetchSprite(goalTiles, goalTilesLen) | ATTR2_PALBANK(PAL_GOAL);
    updateObj(entId);
    return entId;
}
