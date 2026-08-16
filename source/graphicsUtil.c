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

int getZDepth(ObjComponent* obj) {
    Position pos = ((PhysicsComponent*)getComponent(obj->header.entId, obj->posSourceCompType))->pos;
    PositionMini tilePos = {
        (pos.x.WORD / 16 + 0x8000) >> 16,
        (pos.y.WORD / 16 + 0x8000) >> 16,
        (pos.z.WORD / 16 + 0x8000) >> 16,
    };
    int row = tilePos.x + tilePos.z - 7;
    int col = tilePos.x - tilePos.z + 7;
    // multiply the whole thing by 4 so we can control zDepths of things on the same tile
    return 4 * (tilePos.y * 8 * 11 + 8 * row + col / 2 - (gIsZDepthRightToLeft * col)) + getObjZDepthPriority(obj);
}

void updateZDepth(ObjComponent* obj) {
    int zDepth = getZDepth(obj);
    ObjComponent* next = getComponent(obj->nextId, COMP_OBJ);
    ObjComponent* prev = getComponent(obj->prevId, COMP_OBJ);
    ObjComponent* ptr;
    int nextZDepth = next ? getZDepth(next) : INT16_MAX;
    int prevZDepth = prev ? getZDepth(prev) : -1;

    if (next && (zDepth > nextZDepth)) {
        ptr = next;
        while ((ptr->nextId != INT16_MAX) && zDepth > nextZDepth) {
            ptr = getComponent(ptr->nextId, COMP_OBJ);
            nextZDepth = getZDepth(ptr);
        }
        if (prev) prev->nextId = obj->nextId;
        else gDeepestObjEntId = obj->nextId;
        next->prevId = obj->prevId;
        if (zDepth > nextZDepth) { // we reached the back, so insert after ptr
            obj->prevId = ptr->header.entId;
            obj->nextId = INT16_MAX;
            ptr->nextId = obj->header.entId;
            return;
        }
        // insert before ptr
        ObjComponent* newPrev = getComponent(ptr->prevId, COMP_OBJ);
        newPrev->nextId = obj->header.entId;
        obj->prevId = newPrev->header.entId;
        obj->nextId = ptr->header.entId;
        ptr->prevId = obj->header.entId;
    }
    else if (prev && (zDepth < prevZDepth)) {
        ptr = prev;
        while ((ptr->prevId != -1) && zDepth < prevZDepth) {
            ptr = getComponent(ptr->prevId, COMP_OBJ);
            prevZDepth = getZDepth(ptr);
        }
        if (next) next->prevId = obj->prevId;
        prev->nextId = obj->nextId;
        if (zDepth < prevZDepth) { // we reached the front, so insert before ptr
            gDeepestObjEntId = obj->header.entId;
            obj->nextId = ptr->header.entId;
            obj->prevId = -1;
            ptr->prevId = obj->header.entId;
            return;
        }
        obj->prevId = ptr->header.entId;
        ObjComponent* newNext = getComponent(ptr->nextId, COMP_OBJ);
        newNext->prevId = obj->header.entId;
        obj->nextId = newNext->header.entId;
        ptr->nextId = obj->header.entId;
    }
}

void drawSpriteCells() {
    int entId = -1;
    ObjComponent* o = NULL;
    for (int y = 0; y < 8; y++) {
        for (int row = 0; row < 21; row++) {
            for (int col = row & 1; col < 16; col += 2) {
                int z = 7 - col / 2 + row / 2;
                int x = col / 2 + (row + 1) / 2;
                if (colMap[y][z][x]) {
                    entId = reserveEntSlot();
                    addComponentCell(entId, 0, x, y, z);
                    o = addComponentObj(entId, 0, ATTR0_SQUARE, ATTR1_SIZE_32x32,
                        ATTR2_ID(fetchSprite(gCollTileToSpriteMap[colMap[y][z][x]], 512) |
                            ATTR2_PALBANK(2)),
                        8,
                        COMP_CELL);
                }
            }
        }
    }
    if (!o) return;
}
