#include "graphicsUtil.h"

PositionMini getScreenPos(Position pos) {
    PositionMini screenPos = {
        ((pos.x.WORD - pos.z.WORD) >> 16) + 16 * 7,
        // + 16 * 7 is so no negative coords appear on screen
        (((pos.x.WORD + pos.z.WORD) / 2 - pos.y.WORD) >> 16) - 8 * 7
        // - 8 * 7 is so no negative coords appear on screen
    };
    return screenPos;
}

int getZDepth(ObjComponent* obj, bool isNW) {
    Position pos = ((PhysicsComponent*)getComponent(obj->header.entId, obj->posSourceCompType))->pos;
    PositionMini tilePos = {
        (pos.x.WORD / 16 + 0x8000) >> 16,
        (pos.y.WORD / 16 + 0x8000) >> 16,
        (pos.z.WORD / 16 + 0x8000) >> 16,
    };
    // ne: [0], nw: [1]
    return 4 * (
        gLevelData->yHeight *
        (*(&(tilePos.x) + 2 * isNW) * MAP_WIDTH_X + *(&(tilePos.z) - 2 * isNW)) + tilePos.y
        );
}

void updateZDepth(ObjComponent* obj) {
    for (int i = 0; i < 2; i++) { // NE then NW
        int zDepth = getZDepth(obj, i);
        ObjComponent* next = getComponent(obj->nextId[i], COMP_OBJ);
        ObjComponent* prev = getComponent(obj->prevId[i], COMP_OBJ);
        ObjComponent* ptr;
        int nextZDepth = next ? getZDepth(next, i) : INT16_MAX;
        int prevZDepth = prev ? getZDepth(prev, i) : -1;

        if (next && (zDepth > nextZDepth)) {
            ptr = next;
            while ((ptr->nextId[i] != INT16_MAX) && zDepth > nextZDepth) {
                ptr = getComponent(ptr->nextId[i], COMP_OBJ);
                nextZDepth = getZDepth(ptr, i);
            }
            if (prev) prev->nextId[i] = obj->nextId[i];
            else gDeepestObjEntId[i] = obj->nextId[i];
            next->prevId[i] = obj->prevId[i];
            if (zDepth > nextZDepth) { // we reached the back, so insert after ptr
                obj->prevId[i] = ptr->header.entId;
                obj->nextId[i] = INT16_MAX;
                ptr->nextId[i] = obj->header.entId;
                continue;
            }
            // insert before ptr
            ObjComponent* newPrev = getComponent(ptr->prevId[i], COMP_OBJ);
            newPrev->nextId[i] = obj->header.entId;
            obj->prevId[i] = newPrev->header.entId;
            obj->nextId[i] = ptr->header.entId;
            ptr->prevId[i] = obj->header.entId;
        }
        else if (prev && (zDepth < prevZDepth)) {
            ptr = prev;
            while ((ptr->prevId[i] != -1) && zDepth < prevZDepth) {
                ptr = getComponent(ptr->prevId[i], COMP_OBJ);
                prevZDepth = getZDepth(ptr, i);
            }
            if (next) next->prevId[i] = obj->prevId[i];
            prev->nextId[i] = obj->nextId[i];
            if (zDepth < prevZDepth) { // we reached the front, so insert before ptr
                gDeepestObjEntId[i] = obj->header.entId;
                obj->nextId[i] = ptr->header.entId;
                obj->prevId[i] = -1;
                ptr->prevId[i] = obj->header.entId;
                continue;
            }
            obj->prevId[i] = ptr->header.entId;
            ObjComponent* newNext = getComponent(ptr->nextId[i], COMP_OBJ);
            newNext->prevId[i] = obj->header.entId;
            obj->nextId[i] = newNext->header.entId;
            ptr->nextId[i] = obj->header.entId;
        }
    }
}

// this is for when the next/prev ptrs are broken
void resetObjZDepth(ObjComponent* toBeReset) {
    for (int i = 0; i < 2; i++) {
        int zDepth = getZDepth(toBeReset, i);
        toBeReset->prevId[i] = -1;
        toBeReset->nextId[i] = INT16_MAX;
        ObjComponent* currObj = getComponent(gDeepestObjEntId[i], COMP_OBJ);
        if (!currObj) {
            gDeepestObjEntId[i] = toBeReset->header.entId;
            continue;
        }
        ObjComponent* nextObj = getComponent(currObj->nextId[i], COMP_OBJ);
        int currObjZDepth = getZDepth(currObj, i);
        while (nextObj && zDepth < currObjZDepth) {
            currObj = nextObj;
            nextObj = getComponent(nextObj->nextId[i], COMP_OBJ);
            currObjZDepth = getZDepth(currObj, i);
        }
        currObjZDepth = getZDepth(currObj, i);
        if (zDepth > currObjZDepth) {
            toBeReset->nextId[i] = currObj->nextId[i];
            toBeReset->prevId[i] = currObj->header.entId;
            if (nextObj) nextObj->prevId[i] = toBeReset->header.entId;
            currObj->nextId[i] = toBeReset->header.entId;
        }
        else if (zDepth <= currObjZDepth) {
            toBeReset->nextId[i] = currObj->header.entId;
            toBeReset->prevId[i] = currObj->prevId[i];
            ObjComponent* prevObj = getComponent(currObj->prevId[i], COMP_OBJ);
            if (prevObj) prevObj->nextId[i] = toBeReset->header.entId;
            else gDeepestObjEntId[i] = toBeReset->header.entId;
            currObj->prevId[i] = toBeReset->header.entId;
        }
    }
}

void drawSpriteCells() {
    int entId = -1;
    for (int y = 0; y < gLevelData->yHeight; y++) {
        for (int z = 0; z < MAP_WIDTH_Z; z++) {
            for (int x = 0; x < MAP_WIDTH_X; x++) {
                if (gLevelData->clsn[y].cell[z][x]) {
                    int clsnVal = gLevelData->clsn[y].cell[z][x];
                    entId = reserveEntSlot();
                    ComponentHeader* cell = (ComponentHeader*)addComponentCell(entId, 0, x, y, z);
                    Position pos = *(Position*)(cell + 1);
                    PositionMini screenPos = getScreenPos(pos);
                    if (in_range(screenPos.x, 0 - 16, SCREEN_WIDTH + 16) &&
                        in_range(screenPos.y, 0 - 16, SCREEN_HEIGHT + 16))
                        addComponentObj(entId, 0, ATTR0_SQUARE, ATTR1_SIZE_32x32,
                            ATTR2_ID(fetchSprite(gCollTileToSpriteMap[clsnVal], 512) | ATTR2_PALBANK(2)),
                            8,
                            COMP_CELL);
                }
            }
        }
    }
}

void deallocateSpriteCells() {
    for (int i = 0; i < numComps(COMP_CELL); i++)
        markEntToBeDeleted(gCellCompsDense[i].header.entId);
    deleteMarkedEnts();
}
