#include "component.h"
#include "graphicsUtil.h"

ObjComponent* addComponentObj(s16 entId, u16 flags, u16 attr0, u16 attr1, u16 attr2, u16 zDepth, int posSourceCompType) {
    if (gNumCompsPerType[COMP_OBJ] >= MAX_OBJ_COMPONENTS || entId < 0) return NULL;
    ObjComponent newObj = {
        {entId, flags},
        attr0, attr1, attr2,
        zDepth,
        posSourceCompType,
        -1,
        255,
        0
    };
    ObjComponent* obj = &gObjCompsDense[gObjCompDeepestZIndex];
    if (newObj.zDepth < obj->zDepth) {
        newObj.nextIndex = gObjCompDeepestZIndex;
        gObjCompDeepestZIndex = numComps(COMP_OBJ);
        return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
    }
    // iterate through the list of obj comps and insert where z depth is appropriate
    for (int i = 0; i < numComps(COMP_OBJ) - 1; i++) {
        if (newObj.zDepth > obj->zDepth) {
            int nextIdx = obj->nextIndex;
            obj->nextIndex = numComps(COMP_OBJ); // where our new obj will be inserted
            newObj.prevIndex = ((int32_t)obj - (int32_t)&gObjCompsDense) / 16;
            newObj.nextIndex = nextIdx;
            gObjCompsDense[nextIdx].prevIndex = numComps(COMP_OBJ);
            return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
        }
        obj = &gObjCompsDense[obj->nextIndex];
    }
    // if you reach this point, it means newObj is the deepest
    obj->nextIndex = numComps(COMP_OBJ);
    newObj.prevIndex = ((int32_t)obj - (int32_t)&gObjCompsDense) / 16;
    return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
}

void removeComponentObj(int entId) {
    ObjComponent* objComp = getComponent(entId, COMP_OBJ);
    if (!objComp) return;
    gObjCompsDense[objComp->nextIndex].prevIndex = objComp->prevIndex;
    if (objComp->prevIndex == -1)
        gObjCompDeepestZIndex = objComp->nextIndex;
    else
        gObjCompsDense[objComp->prevIndex].nextIndex = objComp->nextIndex;
    stopUsingSprite(objComp->attr2 & ATTR2_ID_MASK);
    removeComponent(entId, COMP_OBJ);
    // todo: fix prevIndex and nextIndex being messed up when an obj with an index less than
    // either of these is removed
}

// updates the pos based on the pos provided by corresponding comp of type posSourceCompType
void updateObjs() {
    for (int i = 0; i < numComps(COMP_OBJ); i++) {
        ObjComponent* objComp = &gObjCompsDense[i];
        if (objComp->posSourceCompType >= NUM_COMP_TYPES) continue;
        PhysicsComponent* physComp = getComponent(objComp->header.entId, objComp->posSourceCompType);
        Position pos = *(Position*)((uint32_t)physComp + sizeof(ComponentHeader));
        const u8* sizes = obj_get_size(getObj(objComp)); // sizes[0,1]: width, height
        bool isDbl = (objComp->header.flags & ATTR0_MODE_MASK) == ATTR0_AFF_DBL;

        PositionMini screenPos = getScreenPos(pos);
        // left of the sprite
        int screenX = screenPos.x - (sizes[0] >> (1 - isDbl));

        // top of the sprite
        int screenY = screenPos.y - (sizes[1] >> (1 - isDbl));

        if (!in_range(screenX, 0 - sizes[0] - isDbl * sizes[0], SCREEN_WIDTH + isDbl * sizes[0] / 2) ||
            !in_range(screenY, 0 - sizes[1] - isDbl * sizes[1], SCREEN_HEIGHT + isDbl * sizes[1] / 2)) {
            objComp->attr0 &= ~ATTR0_MODE_MASK;
            objComp->attr0 |= ATTR0_HIDE;
            continue;
        }
        else {
            objComp->attr0 &= ~(ATTR0_MODE_MASK | ATTR0_GFX_MASK);
            objComp->attr0 |= objComp->header.flags & (ATTR0_MODE_MASK | ATTR0_GFX_MASK);
        }
        objComp->attr0 &= ~ATTR0_Y_MASK;
        objComp->attr1 &= ~ATTR1_X_MASK;
        objComp->attr0 |= ATTR0_Y(screenY) & ATTR0_Y_MASK;
        objComp->attr1 |= ATTR1_X(screenX) & ATTR1_X_MASK;
    }
}
