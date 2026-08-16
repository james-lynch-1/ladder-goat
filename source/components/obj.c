#include "component.h"
#include "graphicsUtil.h"

ObjComponent* addComponentObj(s16 entId, u16 flags, u16 attr0, u16 attr1, u16 attr2, int yOffset, int posSourceCompType) {
    if (gNumCompsPerType[COMP_OBJ] >= MAX_OBJ_COMPONENTS || entId < 0) return NULL;
    ObjComponent newObj = {
        {entId, flags},
        attr0, attr1, attr2,
        -1,
        INT16_MAX,
        yOffset,
        posSourceCompType
    };
    int zDepth = getZDepth(&newObj);
    ObjComponent* currObj = getComponent(gDeepestObjEntId, COMP_OBJ);
    if (!currObj) {
        gDeepestObjEntId = entId;
        return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
    }
    int currObjZDepth = getZDepth(currObj);
    if (zDepth <= currObjZDepth) {
        newObj.nextId = gDeepestObjEntId;
        currObj->prevId = newObj.header.entId;
        gDeepestObjEntId = newObj.header.entId;
        return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
    }
    // iterate through the list of obj comps and insert where zDepth is appropriate
    while ((zDepth > currObjZDepth) && currObj->nextId != INT16_MAX) {
        currObj = getComponent(currObj->nextId, COMP_OBJ);
        currObjZDepth = getZDepth(currObj);
    }
    if (zDepth > currObjZDepth) { // insert newObj at the end
        newObj.prevId = currObj->header.entId;
        newObj.nextId = currObj->nextId;
        currObj->nextId = newObj.header.entId;
        if (newObj.nextId != INT16_MAX)
            ((ObjComponent*)getComponent(newObj.nextId, COMP_OBJ))->prevId = newObj.header.entId;
    }
    else { // insert newObj before currObj, whose zDepth is greater
        newObj.prevId = currObj->prevId;
        newObj.nextId = currObj->header.entId;
        ((ObjComponent*)getComponent(currObj->prevId, COMP_OBJ))->nextId = newObj.header.entId;
        currObj->prevId = newObj.header.entId;
    }
    return (ObjComponent*)addComponentCustom(&newObj, COMP_OBJ);
}

void removeComponentObj(int entId) {
    ObjComponent* objComp = getComponent(entId, COMP_OBJ);
    if (!objComp) return;
    ObjComponent* nextObj = getComponent(objComp->nextId, COMP_OBJ);
    if (nextObj) nextObj->prevId = objComp->prevId;
    if (objComp->prevId == -1)
        gDeepestObjEntId = objComp->nextId;
    else
        ((ObjComponent*)getComponent(objComp->prevId, COMP_OBJ))->nextId = objComp->nextId;
    stopUsingSprite(objComp->attr2 & ATTR2_ID_MASK);
    removeComponent(entId, COMP_OBJ);
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
        int screenY = screenPos.y - sizes[1] + objComp->yOffset;

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

int getObjZDepthPriority(ObjComponent* obj) {
    return obj->header.flags & OBJ_ZDEPTH_PRIO_MASK;
}
