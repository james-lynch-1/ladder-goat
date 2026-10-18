#include "graphicsUtil.h"

ScreenPos getScreenPos(Position pos) {
    ScreenPos screenPos = {
        ((pos.x.WORD - pos.z.WORD) >> 16) - gViewport.x + 16 * gLevelData->zWidth + 16,
        (((pos.x.WORD + pos.z.WORD) / 2 - pos.y.WORD) >> 16) - gViewport.y + 24
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
        ) + getObjZDepthPriority(obj);
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
        for (int z = 0; z < gLevelData->zWidth; z++) {
            for (int x = 0; x < gLevelData->xWidth; x++) {
                if (gLevelData->clsn[y].cell[z][x]) {
                    int clsnVal = gLevelData->clsn[y].cell[z][x];
                    entId = reserveEntSlot();
                    CellComponent* cell = addComponentCell(entId, 0, x, y, z, clsnVal);
                    gColl[y][z][x] = cell;
                    Position pos = *(Position*)((u8*)cell + sizeof(ComponentHeader));
                    ScreenPos screenPos = getScreenPos(pos);
                    bool isOffScreen = !in_range(screenPos.x, 0 - 16, SCREEN_WIDTH + 16) ||
                        !in_range(screenPos.y, 0 - 16, SCREEN_HEIGHT + 16);
                    addComponentObj(entId,
                        0,
                        ATTR0_SQUARE | (isOffScreen << 9),
                        ATTR1_SIZE_32x32 | gCollTileToSpriteMap[clsnVal].flipFlags,
                        ATTR2_ID(fetchSprite(gCollTileToSpriteMap[clsnVal].tiles, 512)) |
                        ATTR2_PALBANK(gCollTileToSpriteMap[clsnVal].palIndex),
                        0,
                        COMP_CELL);
                }
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
