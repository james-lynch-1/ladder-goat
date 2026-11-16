#include "graphicsUtil.h"

ScreenPos getScreenPos(Position pos) {
    ScreenPos screenPos = {
        ((pos.x.WORD - pos.z.WORD) >> 16) - gViewport.x + 16 * gLevelData->zWidth + 16,
        (((pos.x.WORD + pos.z.WORD) / 2 - pos.y.WORD) >> 16) - gViewport.y + 24
    };
    return screenPos;
}

ScreenPos getScreenPosFromOldViewport(Position pos) {
    ScreenPos screenPos = {
        ((pos.x.WORD - pos.z.WORD) >> 16) - gOldViewport.x + 16 * gLevelData->zWidth + 16,
        (((pos.x.WORD + pos.z.WORD) / 2 - pos.y.WORD) >> 16) - gOldViewport.y + 24
    };
    return screenPos;
}

ScreenPos getScreenPosFromViewportXY(Position pos, int levelIndex, int vpX, int vpY) {
    ScreenPos screenPos = {
        ((pos.x.WORD - pos.z.WORD) >> 16) - vpX + 16 * gLevels[levelIndex]->zWidth + 16,
        (((pos.x.WORD + pos.z.WORD) / 2 - pos.y.WORD) >> 16) - vpY + 24
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
    return 4 * getClsnOffsFromTilePos(tilePos) + getObjZDepthPriority(obj);
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

// this is for when the next/prev ptrs are broken
void resetObjZDepth(ObjComponent* toBeReset) {
    for (int i = 0; i < 1; i++) {
        int zDepth = getZDepth(toBeReset);
        toBeReset->prevId = -1;
        toBeReset->nextId = INT16_MAX;
        ObjComponent* currObj = getComponent(gDeepestObjEntId, COMP_OBJ);
        if (!currObj) {
            gDeepestObjEntId = toBeReset->header.entId;
            continue;
        }
        ObjComponent* nextObj = getComponent(currObj->nextId, COMP_OBJ);
        int currObjZDepth = getZDepth(currObj);
        while (nextObj && zDepth < currObjZDepth) {
            currObj = nextObj;
            nextObj = getComponent(nextObj->nextId, COMP_OBJ);
            currObjZDepth = getZDepth(currObj);
        }
        currObjZDepth = getZDepth(currObj);
        if (zDepth > currObjZDepth) {
            toBeReset->nextId = currObj->nextId;
            toBeReset->prevId = currObj->header.entId;
            if (nextObj) nextObj->prevId = toBeReset->header.entId;
            currObj->nextId = toBeReset->header.entId;
        }
        else if (zDepth <= currObjZDepth) {
            toBeReset->nextId = currObj->header.entId;
            toBeReset->prevId = currObj->prevId;
            ObjComponent* prevObj = getComponent(currObj->prevId, COMP_OBJ);
            if (prevObj) prevObj->nextId = toBeReset->header.entId;
            else gDeepestObjEntId = toBeReset->header.entId;
            currObj->prevId = toBeReset->header.entId;
        }
    }
}

void drawSpriteCells() {
    int entId = -1;
    u8* ptr = (u8*)gLevelData->clsn;
    int i = 0;
    for (int y = 0; y < gLevelData->yHeight; y++) {
        for (int z = 0; z < gLevelData->zWidth; z++) {
            for (int x = 0; x < gLevelData->xWidth; x++) {
                if (*ptr != 0) {
                    entId = reserveEntSlot();
                    CellComponent* cell = addComponentCell(entId, 0, x, y, z, *ptr);
                    gClsn[i] = (ComponentHeader*)cell;
                    Position pos = *(Position*)((u8*)cell + sizeof(ComponentHeader));
                    ScreenPos screenPos = getScreenPos(pos);
                    bool isOffScreen = !in_range(screenPos.x, 0 - 16, SCREEN_WIDTH + 16) ||
                        !in_range(screenPos.y, 0 - 16, SCREEN_HEIGHT + 16);
                    addComponentObj(entId,
                        0,
                        ATTR0_SQUARE | (isOffScreen << 9),
                        ATTR1_SIZE_32x32 | gCollTileToSpriteMap[*ptr].flipFlags,
                        ATTR2_ID(fetchSprite(gCollTileToSpriteMap[*ptr].tiles, 512)) |
                        ATTR2_PALBANK(gCollTileToSpriteMap[*ptr].palIndex),
                        0,
                        COMP_CELL);
                }
                ptr++;
                i++;
            }
        }
    }
}

const int hFlip = SE_HFLIP | (SE_HFLIP << 16);
const int vFlip = SE_VFLIP | (SE_VFLIP << 16);
const int mapPal = SE_PALBANK(PAL_MAP) | (SE_PALBANK(PAL_MAP) << 16);
const u32 twoBlackTop = 0x00020001 | mapPal;
const u32 twoBlackTopMirrorHorz = 0x00010002 | hFlip | mapPal;
const u32 twoBlackTopMirrorVert = 0x00020001 | vFlip | mapPal;
const u32 twoBlackTopMirrorHorzVert = 0x00010002 | hFlip | vFlip | mapPal;
const u32 twoWhiteTop = 0x00040003 | mapPal;
const u32 twoWhiteTopMirrorHorz = 0x00030004 | hFlip | mapPal;
const u32 twoWhiteTopMirrorVert = 0x00040003 | vFlip | mapPal;
const u32 twoWhiteTopMirrorHorzVert = 0x00030004 | hFlip | vFlip | mapPal;
const u32 blackTopWhiteBtm = 0x00060005 | mapPal;
const u32 blackTopWhiteBtmMirrorHorz = 0x00050006 | hFlip | mapPal;
const u32 blackTopWhiteBtmMirrorVert = 0x00060005 | vFlip | mapPal;
const u32 blackTopWhiteBtmMirrorHorzVert = 0x00050006 | hFlip | vFlip | mapPal;

const u32 tilePairArr[6][2] = {
    { twoBlackTop, twoWhiteTop },
    { twoBlackTopMirrorHorz, twoWhiteTopMirrorHorz },
    { twoBlackTopMirrorVert, twoWhiteTopMirrorVert },
    { twoBlackTopMirrorHorzVert, twoWhiteTopMirrorHorzVert },
    { blackTopWhiteBtm, blackTopWhiteBtmMirrorHorz },
    { blackTopWhiteBtmMirrorVert, blackTopWhiteBtmMirrorHorzVert }
};

void drawBgTileRow(int isBtm) {
    const int xOffs = 2 * (gLevelData->xWidth + 1);
    const int zOffs = 2 * gLevelData->zWidth;
    const int vpX = ((gViewport.x & ~0b1111) - 8 * zOffs) / 8;
    int mapY = (gViewport.y & ~0b111) / 8 + isBtm * 20;
    memset32(&se_mat[SBB_MAP][((u32)mapY) % 32], 0, 16);
    for (int mapX = vpX; mapX < vpX + 32; mapX += 2) {
        int isEvenX = !((mapX / 2) % 2);
        int isEvenY = !(mapY % 2);
        if (mapY >= mapX / 2 &&
            mapY > -(mapX / 2) &&
            mapY <= mapX / 2 + zOffs &&
            mapY < -(mapX / 2) + xOffs
            ) {
            bool isTopLeft = mapY == (-(mapX / 2) + 1);
            bool isTopRight = mapY == mapX / 2;
            bool isBtmLeft = mapY == mapX / 2 + zOffs;
            bool isBtmRight = mapY == (-(mapX / 2) + xOffs - 1);
            u32 tilePair;
            if (isTopLeft) tilePair = tilePairArr[0][!isEvenX];
            else if (isTopRight) tilePair = tilePairArr[1][isEvenX];
            else if (isBtmLeft) tilePair = tilePairArr[2][!isEvenX];
            else if (isBtmRight) tilePair = tilePairArr[3][isEvenX];
            else if (isEvenY) tilePair = tilePairArr[4][!isEvenX];
            else tilePair = tilePairArr[5][!isEvenX];
            *(u32*)&se_mat[SBB_MAP][mapY % 32][(mapX + zOffs) % 32] = tilePair;
            if (isBtmRight | isTopRight) break;
        }
    }
}

void drawBgTileCol(int isRight) {
    const int xOffs = 2 * (gLevelData->xWidth + 1);
    const int zOffs = 2 * gLevelData->zWidth;
    const int vpX = ((gViewport.x & ~0b1111) - 8 * zOffs) / 8;
    const int mapX = vpX + isRight * 30;
    for (int i = 0; i < 33; i++)
        memset32(&se_mat[SBB_MAP][((gViewport.y) / 8 + i) % 32][(u32)(mapX + zOffs) % 32], 0, 1);
    const int vpY = (gViewport.y & ~0b111) / 8;
    for (int mapY = vpY; mapY < vpY + 22; mapY++) {
        int isEvenX = !((mapX / 2) % 2);
        int isEvenY = !(mapY % 2);
        if (mapY >= mapX / 2 &&
            mapY > -(mapX / 2) &&
            mapY <= mapX / 2 + zOffs &&
            mapY < -(mapX / 2) + xOffs
            ) {
            bool isTopLeft = mapY == (-(mapX / 2) + 1);
            bool isTopRight = mapY == mapX / 2;
            bool isBtmLeft = mapY == mapX / 2 + zOffs;
            bool isBtmRight = mapY == (-(mapX / 2) + xOffs - 1);
            u32 tilePair;
            if (isTopLeft) tilePair = tilePairArr[0][!isEvenX];
            else if (isTopRight) tilePair = tilePairArr[1][isEvenX];
            else if (isBtmLeft) tilePair = tilePairArr[2][!isEvenX];
            else if (isBtmRight) tilePair = tilePairArr[3][isEvenX];
            else if (isEvenY) tilePair = tilePairArr[4][!isEvenX];
            else tilePair = tilePairArr[5][!isEvenX];
            *(u32*)&se_mat[SBB_MAP][mapY % 32][(mapX + zOffs) % 32] = tilePair;
        }
    }
}

void drawBgTiles() {
    memset32(&se_mem[SBB_MAP], 0, SBB_SIZE / 4);
    const int xOffs = 2 * (gLevelData->xWidth + 1);
    const int zOffs = 2 * gLevelData->zWidth;
    const int vpY = (gViewport.y & ~0b111) / 8;
    const int vpX = ((gViewport.x & ~0b1111) - 8 * zOffs) / 8;
    for (int mapY = max(vpY, 0); mapY < vpY + 22; mapY++) {
        for (int mapX = vpX; mapX < vpX + 32; mapX += 2) {
            int isEvenX = !((mapX / 2) % 2);
            int isEvenY = !(mapY % 2);
            if (mapY >= mapX / 2 &&
                mapY > -(mapX / 2) &&
                mapY <= mapX / 2 + zOffs &&
                mapY < -(mapX / 2) + xOffs
                ) {
                bool isTopLeft = mapY == (-(mapX / 2) + 1);
                bool isTopRight = mapY == mapX / 2;
                bool isBtmLeft = mapY == mapX / 2 + zOffs;
                bool isBtmRight = mapY == (-(mapX / 2) + xOffs - 1);
                u32 tilePair;
                if (isTopLeft) tilePair = tilePairArr[0][!isEvenX];
                else if (isTopRight) tilePair = tilePairArr[1][isEvenX];
                else if (isBtmLeft) tilePair = tilePairArr[2][!isEvenX];
                else if (isBtmRight) tilePair = tilePairArr[3][isEvenX];
                else if (isEvenY) tilePair = tilePairArr[4][!isEvenX];
                else tilePair = tilePairArr[5][!isEvenX];
                *(u32*)&se_mat[SBB_MAP][mapY % 32][(mapX + zOffs) % 32] = tilePair;
                if (isBtmRight | isTopRight) break;
            }
        }
    }
}

void scroll() {
    int scrollYDiff = (gViewport.y & ~0b111) / 8 - (gOldViewport.y & ~0b111) / 8;
    if (scrollYDiff) {
        drawBgTileRow(scrollYDiff > 0);
    }

    int scrollXDiff = (gViewport.x & ~0b1111) / 16 - (gOldViewport.x & ~0b1111) / 16;
    if (scrollXDiff) {
        drawBgTileCol(scrollXDiff > 0);
    }
}

void deallocateSpriteCells() {
    for (int i = 0; i < numComps(COMP_CELL); i++)
        markEntToBeDeleted(gCellCompsDense[i].header.entId);
    deleteMarkedEnts();
}
