#include "component.h"
#include "graphicsUtil.h"

ObjComponent* addComponentObj(s16 entId, u16 flags, u16 attr0, u16 attr1, u16 attr2, int yOffset, int posSourceCompType) {
    if (gNumCompsPerType[COMP_OBJ] >= MAX_OBJ_COMPONENTS || entId < 0) return NULL;
    ObjComponent newObj = {
        {entId, flags},
        attr0, attr1, attr2,
        yOffset,
        posSourceCompType,
        {-1, -1},
        {INT16_MAX, INT16_MAX}
    };
    for (int i = 0; i < 2; i++) {
        int zDepth = getZDepth(&newObj, i);
        ObjComponent* objIter = getComponent(gDeepestObjEntId[i], COMP_OBJ);
        if (!objIter) {
            gDeepestObjEntId[i] = entId;
            continue;
        }
        int objIterZDepth = getZDepth(objIter, i);
        if (zDepth <= objIterZDepth) {
            newObj.nextId[i] = gDeepestObjEntId[i];
            objIter->prevId[i] = newObj.header.entId;
            gDeepestObjEntId[i] = newObj.header.entId;
            continue;
        }
        // iterate through the list of obj comps and insert where zDepth is appropriate
        while ((zDepth > objIterZDepth) && objIter->nextId[i] != INT16_MAX) {
            objIter = getComponent(objIter->nextId[i], COMP_OBJ);
            objIterZDepth = getZDepth(objIter, i);
        }
        if (zDepth > objIterZDepth) { // insert newObj at the end
            newObj.prevId[i] = objIter->header.entId;
            newObj.nextId[i] = objIter->nextId[i];
            objIter->nextId[i] = newObj.header.entId;
            if (newObj.nextId[i] != INT16_MAX)
                ((ObjComponent*)getComponent(newObj.nextId[i], COMP_OBJ))->prevId[i] = newObj.header.entId;
        }
        else { // insert newObj before currObj, whose zDepth is greater
            newObj.prevId[i] = objIter->prevId[i];
            newObj.nextId[i] = objIter->header.entId;
            ((ObjComponent*)getComponent(objIter->prevId[i], COMP_OBJ))->nextId[i] = newObj.header.entId;
            objIter->prevId[i] = newObj.header.entId;
        }
    }
    return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
}

void removeComponentObj(int entId) {
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    if (!obj) return;
    for (int i = 0; i < 2; i++) {
        ObjComponent* nextObj = getComponent(obj->nextId[i], COMP_OBJ);
        if (nextObj) nextObj->prevId[i] = obj->prevId[i];
        if (obj->prevId[i] == -1)
            gDeepestObjEntId[i] = obj->nextId[i];
        else
            ((ObjComponent*)getComponent(obj->prevId[i], COMP_OBJ))->nextId[i] = obj->nextId[i];
    }
    stopUsingSprite(obj->attr2 & ATTR2_ID_MASK);
    removeComponent(entId, COMP_OBJ);
}

void hideObj(ObjComponent* obj) {
    obj->attr0 &= ~ATTR0_MODE_MASK;
    obj->attr0 |= ATTR0_HIDE;
}

void unhideObj(ObjComponent* obj) {
    obj->attr0 &= ~(ATTR0_MODE_MASK | ATTR0_GFX_MASK);
    obj->attr0 |= obj->header.flags & (ATTR0_MODE_MASK | ATTR0_GFX_MASK);
}

void changePalette(ObjComponent* obj, enum Palette pal) {
    if (obj) {
        obj->attr2 &= ~ATTR2_PALBANK_MASK;
        obj->attr2 |= ATTR2_PALBANK(pal);
    }
}

void updateObj(int entId) {
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    if (obj->posSourceCompType >= NUM_COMP_TYPES) return;
    PhysicsComponent* physComp = getComponent(obj->header.entId, obj->posSourceCompType);
    Position pos = *(Position*)((uint32_t)physComp + sizeof(ComponentHeader));
    const u8* sizes = obj_get_size((OBJ_ATTR*)((u8*)obj + sizeof(ComponentHeader))); // sizes[0,1]: width, height
    bool isDbl = (obj->header.flags & ATTR0_MODE_MASK) == ATTR0_AFF_DBL;

    PositionMini screenPos = getScreenPos(pos);

    // left of the sprite
    int screenX = screenPos.x - (sizes[0] >> (1 - isDbl));
    // top of the sprite
    int screenY = screenPos.y - sizes[1] + obj->yOffset;

    if (isEntInactive(entId) ||
        !in_range(screenX, 0 - sizes[0] - isDbl * sizes[0], SCREEN_WIDTH + isDbl * sizes[0] / 2) ||
        !in_range(screenY, 0 - sizes[1] - isDbl * sizes[1], SCREEN_HEIGHT + isDbl * sizes[1] / 2)) {
        hideObj(obj);
    }
    else
        unhideObj(obj);
    obj->attr0 &= ~ATTR0_Y_MASK;
    obj->attr1 &= ~ATTR1_X_MASK;
    obj->attr0 |= ATTR0_Y(screenY) & ATTR0_Y_MASK;
    obj->attr1 |= ATTR1_X(screenX) & ATTR1_X_MASK;
}

int getObjZDepthPriority(ObjComponent* obj) {
    return (obj->header.flags & OBJ_ZDEPTH_PRIO_MASK) >> OBJ_ZDEPTH_PRIO_SHIFT;
}
