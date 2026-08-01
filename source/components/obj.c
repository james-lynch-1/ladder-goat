#include "component.h"

ObjComponent* addComponentObj(s16 entId, u16 flags, int posSourceCompType) {
    if (gNumCompsPerType[COMP_OBJ] >= MAX_OBJ_COMPONENTS || entId < 0) return NULL;
    ObjComponent o = {
        {entId, flags},
        gNumCompsPerType[COMP_OBJ],
        posSourceCompType
    };
    ObjComponent* objComp = (ObjComponent*)addComponentCustom(&o, COMP_OBJ);
    return objComp;
}

void removeComponentObj(int entId) {
    ObjComponent* objComp = getComponent(entId, COMP_OBJ);
    if (!objComp) return;
    int objIndex = objComp->objIndex;
    OBJ_ATTR* thisObjBufferPtr = getObj(objComp);
    stopUsingSprite(getObj(objComp)->attr2 & ATTR2_ID_MASK);

    // replace this one with the last obj in the buffer,
    // also update that obj's obj comp to point to this new location
    int replacementEntId = removeComponent(entId, COMP_OBJ);

    // if there's one obj left, or it's the last obj in the dense array, just clear its attrs
    if (replacementEntId == entId) {
        getObj(objComp)->attr0 = 512;
        getObj(objComp)->attr1 = 0;
        getObj(objComp)->attr2 = 0;
        return;
    }

    // move the replacement obj buffer attrs to their new location
    ObjComponent* replacementObj = getComponent(replacementEntId, COMP_OBJ);
    OBJ_ATTR* replacementObjBufferPtr = getObj(replacementObj);
    memcpy16(thisObjBufferPtr, replacementObjBufferPtr, 3);

    // finally, clear the replacement's old attrs
    getObj(replacementObj)->attr0 = 512;
    getObj(replacementObj)->attr1 = 0;
    getObj(replacementObj)->attr2 = 0;

    // make the replacement obj component point to this buffer location
    replacementObj->objIndex = objIndex;
}

// updates the pos based on the pos provided by corresponding comp of type posSourceCompType
void updateObjs() {
    for (int i = 0; i < gNumCompsPerType[COMP_OBJ]; i++) {
        ObjComponent* objComp = &gObjCompsDense[i];
        OBJ_ATTR* obj = getObj(objComp);
        PhysicsComponent* physComp = getComponent(objComp->header.entId, objComp->posSourceCompType);
        Position pos = *(Position*)((uint32_t)physComp + sizeof(ComponentHeader));
        const u8* sizes = obj_get_size(getObj(objComp)); // sizes[0,1]: width, height
        bool isDbl = (objComp->header.flags & ATTR0_MODE_MASK) == ATTR0_AFF_DBL;
        // left of the sprite
        int screenX = ((pos.x.WORD - pos.z.WORD) >> 16) - (sizes[0] >> (1 - isDbl));
        // top of the sprite
        int screenY = (((pos.x.WORD + pos.z.WORD) / 2 - pos.y.WORD) >> 16) - (sizes[1] >> (1 - isDbl));
        if (!in_range(screenX, 0 - sizes[0] - isDbl * sizes[0], SCREEN_WIDTH + isDbl * sizes[0] / 2) ||
            !in_range(screenY, 0 - sizes[1] - isDbl * sizes[1], SCREEN_HEIGHT + isDbl * sizes[1] / 2)) {
            obj->attr0 &= ~ATTR0_MODE_MASK;
            obj->attr0 |= ATTR0_HIDE;
        }
        else {
            obj->attr0 &= ~ATTR0_MODE_MASK;
            obj->attr0 |= objComp->header.flags & ATTR0_MODE_MASK;
        }
        obj->attr0 &= ~ATTR0_Y_MASK;
        obj->attr1 &= ~ATTR1_X_MASK;
        obj->attr0 |= ATTR0_Y(screenY) & ATTR0_Y_MASK;
        obj->attr1 |= ATTR1_X(screenX) & ATTR1_X_MASK;
    }
}
