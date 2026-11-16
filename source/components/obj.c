#include "component.h"
#include "graphicsUtil.h"

ObjComponent* addComponentObj(s16 entId, u16 flags, u16 attr0, u16 attr1, u16 attr2, int yOffset, int posSourceCompType) {
    if (gNumCompsPerType[COMP_OBJ] >= MAX_OBJ_COMPONENTS || entId < 0) return NULL;
    ObjComponent newObj = {
        { entId, flags },
        attr0, attr1, attr2,
        yOffset,
        posSourceCompType,
        -1,
        INT16_MAX,
    };
    newObj.attr2 |= ATTR2_PRIO(PRIO_MAP);
    for (int i = 0; i < 1; i++) {
        int zDepth = getZDepth(&newObj);
        ObjComponent* objIter = getComponent(gDeepestObjEntId, COMP_OBJ);
        if (!objIter) {
            gDeepestObjEntId = entId;
            continue;
        }
        int objIterZDepth = getZDepth(objIter);
        if (zDepth <= objIterZDepth) {
            newObj.nextId = gDeepestObjEntId;
            objIter->prevId = newObj.header.entId;
            gDeepestObjEntId = newObj.header.entId;
            continue;
        }
        // iterate through the list of obj comps and insert where zDepth is appropriate
        while ((zDepth > objIterZDepth) && objIter->nextId != INT16_MAX) {
            objIter = getComponent(objIter->nextId, COMP_OBJ);
            objIterZDepth = getZDepth(objIter);
        }
        if (zDepth > objIterZDepth) { // insert newObj at the end
            newObj.prevId = objIter->header.entId;
            newObj.nextId = objIter->nextId;
            objIter->nextId = newObj.header.entId;
            if (newObj.nextId != INT16_MAX)
                ((ObjComponent*)getComponent(newObj.nextId, COMP_OBJ))->prevId = newObj.header.entId;
        }
        else { // insert newObj before currObj, whose zDepth is greater
            newObj.prevId = objIter->prevId;
            newObj.nextId = objIter->header.entId;
            ((ObjComponent*)getComponent(objIter->prevId, COMP_OBJ))->nextId = newObj.header.entId;
            objIter->prevId = newObj.header.entId;
        }
    }
    return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
}

void removeComponentObj(int entId) {
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    if (!obj) return;
    for (int i = 0; i < 2; i++) {
        ObjComponent* nextObj = getComponent(obj->nextId, COMP_OBJ);
        if (nextObj) nextObj->prevId = obj->prevId;
        if (obj->prevId == -1)
            gDeepestObjEntId = obj->nextId;
        else
            ((ObjComponent*)getComponent(obj->prevId, COMP_OBJ))->nextId = obj->nextId;
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

void updateObjUsingPtr(ObjComponent* obj) {
    if (obj->posSourceCompType >= NUM_COMP_TYPES) return;
    PhysicsComponent* physComp = getComponent(obj->header.entId, obj->posSourceCompType);
    Position pos = *(Position*)(&physComp->pos);
    const u8* sizes = obj_get_size((OBJ_ATTR*)(&obj->attr0)); // sizes[0,1]: width, height
    bool isDbl = (obj->header.flags & ATTR0_MODE_MASK) == ATTR0_AFF_DBL;

    ScreenPos screenPos = getScreenPos(pos);

    // left of the sprite
    int screenX = screenPos.x - (sizes[0] >> !isDbl);
    // top of the sprite
    int screenY = screenPos.y - sizes[1] + obj->yOffset;

    if (!in_range(screenX, -96, SCREEN_WIDTH) ||
        !in_range(screenY, -64, SCREEN_HEIGHT) ||
        isEntInactive(obj->header.entId)
        ) {
        hideObj(obj);
    }
    else
        unhideObj(obj);
    obj->attr0 &= ~ATTR0_Y_MASK;
    obj->attr1 &= ~ATTR1_X_MASK;
    obj->attr0 |= ATTR0_Y(screenY) & ATTR0_Y_MASK;
    obj->attr1 |= ATTR1_X(screenX) & ATTR1_X_MASK;
}

void updateObj(int entId) {
    ObjComponent* obj = getComponent(entId, COMP_OBJ);
    updateObjUsingPtr(obj);
}

void updateObjs() {
    for (int i = 0; i < numComps(COMP_OBJ); i++) {
        ObjComponent* obj = &gObjCompsDense[i];
        if (obj->posSourceCompType >= NUM_COMP_TYPES) return;
        PhysicsComponent* physComp = getComponent(obj->header.entId, obj->posSourceCompType);
        Position pos = *(Position*)(&physComp->pos);
        const u8* sizes = obj_get_size((OBJ_ATTR*)(&obj->attr0)); // sizes[0,1]: width, height
        bool isDbl = (obj->header.flags & ATTR0_MODE_MASK) == ATTR0_AFF_DBL;

        ScreenPos screenPos = getScreenPos(pos);

        // left of the sprite
        int screenX = screenPos.x - (sizes[0] >> !isDbl);
        // top of the sprite
        int screenY = screenPos.y - sizes[1] + obj->yOffset;

        if (!in_range(screenX, -96, SCREEN_WIDTH) ||
            !in_range(screenY, -64, SCREEN_HEIGHT) ||
            isEntInactive(obj->header.entId)
            ) {
            hideObj(obj);
        }
        else
            unhideObj(obj);
        obj->attr0 &= ~ATTR0_Y_MASK;
        obj->attr1 &= ~ATTR1_X_MASK;
        obj->attr0 |= ATTR0_Y(screenY) & ATTR0_Y_MASK;
        obj->attr1 |= ATTR1_X(screenX) & ATTR1_X_MASK;
    }
}

int getObjZDepthPriority(ObjComponent* obj) {
    return (obj->header.flags & OBJ_ZDEPTH_PRIO_MASK) >> OBJ_ZDEPTH_PRIO_SHIFT;
}
