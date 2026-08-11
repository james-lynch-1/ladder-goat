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

void initSpriteCells() { // call after initialising player
    int id = reserveEntSlot();
    gSpriteCellStartingObjCompDenseIdx = numComps(COMP_OBJ);
    gObjCompDeepestZIndex = numComps(COMP_OBJ);
    addComponentCell(id, 0, 0, 0, 0);
    addComponentObj(id, 0, 0, ATTR1_SIZE_32, ATTR2_PALBANK(2), 0, COMP_CELL);
    const int indexesThatShouldBeHalfWidth[NUM_SPRITE_CELLS] = {
        0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1
    };
    for (int i = 1; i < NUM_SPRITE_CELLS; i++) {
        int entId = reserveEntSlot();
        addComponentCell(entId, 0, 0, 0, 0);
        ObjComponent* o = addComponentObj(entId, 0,
            ATTR0_SQUARE,
            ATTR1_SIZE_32,
            ATTR2_PALBANK(2),
            0, COMP_CELL);
        if (indexesThatShouldBeHalfWidth[i]) {
            o->attr0 |= ATTR0_TALL;
            o->attr1 |= ATTR1_SIZE_16x32;
        }
    }
    for (int i = 0; i < 5; i++) {
        int entId = reserveEntSlot();
        addComponentCell(entId, 0, 0, 0, 0);
        addComponentObj(entId, ATTR0_WINDOW,
            ATTR0_SQUARE | ATTR0_WINDOW,
            ATTR1_SIZE_32,
            fetchSprite(spriteCellPurpleTiles, spriteCellPurpleTilesLen),
            0, COMP_CELL);
    }
    drawSpriteCells(((PhysicsComponent*)getComponent(gLadderId, COMP_PHYSICS))->pos, false);
}

void drawSpriteCells(Position pos, bool isCellsRightToLeft) {
    int playerYTile = (pos.y.HALF.HI) / 16;
    int playerXTile = (pos.x.HALF.HI + 8) / 16; // +8 for rounding to nearest tile
    int playerZTile = (pos.z.HALF.HI + 8) / 16;
    const int xZCoords[NUM_SPRITE_CELLS][2] = {
        // full windows at: [-3, -3] 
        {-3, -3}, // r0
        {-3, -2}, // r1
        {-2, -3},
        {-3, -1}, // r2
        {-2, -2},
        {-1, -3},
        {-2, -1}, // r3
        {-1, -2},
        {-2, 0}, // r4
        {-1, -1},
        {0, -2},
        {-1, 0}, // r5
        {0, -1},
        {-1, 1}, // r6
        {0, 0},
        {1, -1},
        {0, 1}, // r7
        {1, 0},
        {0, 2}, // r8
        {1, 1},
        {2, 0},
        {1, 2}, // r9
        {2, 1},
        {1, 3}, // r10
        {2, 2},
        {3, 1}
    };
    const int indexesThatShouldBeHalfWidth[NUM_SPRITE_CELLS] = {
        0, 0, 0, 4, 0, -4, 0, 0, 4, 0, -4, 0, 0, 4, 0, -4, 0, 0, 4, 0, -4, 0, 0, 4, 0, -4
    };

    ObjComponent* start[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    ObjComponent* end[8] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
    for (int cell = 0; cell < NUM_SPRITE_CELLS; cell++) {
        int x = playerXTile + xZCoords[cell][isCellsRightToLeft];
        int z = playerZTile + xZCoords[cell][1 - isCellsRightToLeft];
        bool isCellOccupied = false;
        int modifier = 0;
        CellComponent* cellComp = getComponent(gObjCompsDense[gSpriteCellStartingObjCompDenseIdx + cell].header.entId, COMP_CELL);
        for (int y = 7; y >= playerYTile; y--) {
            int colVal = (z + y < 17) && (x + y < 17) ? colMap[y][z + y][x + y] : 0;
            int spriteSize = 512;
            if (colVal) {
                if (!start[y]) {
                    end[y] = start[y] = &gObjCompsDense[gSpriteCellStartingObjCompDenseIdx + cell];
                    end[y]->nextIndex = INT8_MAX;
                }
                else {
                    int prevIndex = ((int32_t)end[y] - (int32_t)&gObjCompsDense) / 16;
                    end[y]->nextIndex = gSpriteCellStartingObjCompDenseIdx + cell;
                    end[y] = &gObjCompsDense[gSpriteCellStartingObjCompDenseIdx + cell];
                    end[y]->prevIndex = prevIndex;
                }
                if (indexesThatShouldBeHalfWidth[cell]) {
                    modifier = indexesThatShouldBeHalfWidth[cell] * (isCellsRightToLeft ? -1 : 1); // pos on left, neg on right
                    colVal += 128;
                    spriteSize = 256;
                    if ((!isCellsRightToLeft && (indexesThatShouldBeHalfWidth[cell] == 4)) ||
                        (isCellsRightToLeft && (indexesThatShouldBeHalfWidth[cell] == -4)))
                        end[y]->attr1 |= ATTR1_HFLIP;
                    else end[y]->attr1 &= ~ATTR1_FLIP_MASK;
                }
                end[y]->zDepth = y * NUM_SPRITE_CELLS + cell;
                end[y]->header.flags &= ~ATTR0_MODE_MASK;
                if ((end[y]->attr2 & ATTR2_ID_MASK) != colVal) {
                    // change the sprite if it's different to before
                    if ((end[y]->attr2 & ATTR2_ID_MASK))
                        stopUsingSprite(end[y]->attr2 & ATTR2_ID_MASK);
                    end[y]->attr2 = end[y]->attr2 & ~ATTR2_ID_MASK;
                    end[y]->attr2 |= fetchSprite(gCollTileToSpriteMap[colVal], spriteSize) & ATTR2_ID_MASK;
                }
                isCellOccupied = true;
                break;
            }
        }
        if (!isCellOccupied) {
            // connect the empty to the y=0 linked list
            if (!start[playerYTile])
                end[playerYTile] = start[playerYTile] = &gObjCompsDense[gSpriteCellStartingObjCompDenseIdx + cell];
            else {
                end[playerYTile]->nextIndex = gSpriteCellStartingObjCompDenseIdx + cell;
                end[playerYTile] = &gObjCompsDense[gSpriteCellStartingObjCompDenseIdx + cell];
            }
            end[playerYTile]->header.flags |= ATTR0_HIDE;
            end[playerYTile]->zDepth = cell;
            if (indexesThatShouldBeHalfWidth[cell]) {
                modifier = indexesThatShouldBeHalfWidth[cell] * (isCellsRightToLeft ? -1 : 1); // pos on left, neg on right
                if ((!isCellsRightToLeft && (indexesThatShouldBeHalfWidth[cell] == 4)) ||
                    (isCellsRightToLeft && (indexesThatShouldBeHalfWidth[cell] == -4)))
                    end[playerYTile]->attr1 |= ATTR1_HFLIP;
                else end[playerYTile]->attr1 &= ~ATTR1_FLIP_MASK;
            }
        }
        cellComp->pos.y.HALF.HI = 8;
        cellComp->pos.x.HALF.HI = x * 16 + modifier;
        cellComp->pos.z.HALF.HI = z * 16 - modifier;
    }

    // add the window sprites on top of the top sprite cells (prevent top faces from drawing if
    // they are meant to be occluded)
    int spriteMaskCoordOffsets[5][2] = { {-4, -4}, {-4, -3}, {-3, -4}, {-4, -2}, {-2, -4} };
    for (int i = 0; i < 5; i++) {
        int prevIndex = ((int32_t)end[playerYTile] - (int32_t)&gObjCompsDense) / 16;
        end[playerYTile]->nextIndex = gSpriteCellStartingObjCompDenseIdx + NUM_SPRITE_CELLS + i;
        end[playerYTile] = &gObjCompsDense[gSpriteCellStartingObjCompDenseIdx + NUM_SPRITE_CELLS + i];
        end[playerYTile]->prevIndex = prevIndex;
        CellComponent* cellComp = getComponent(end[playerYTile]->header.entId, COMP_CELL);
        cellComp->pos.y.HALF.HI = 8;
        cellComp->pos.x.HALF.HI = (playerXTile + spriteMaskCoordOffsets[i][0]) * 16;
        cellComp->pos.z.HALF.HI = (playerZTile + spriteMaskCoordOffsets[i][1]) * 16;
    }

    bool foundDeepest = false;
    for (int i = 0; i < 7; i++) { // join the layers
        if (end[i]) { // if there are any cells in this layer
            if (!foundDeepest) {
                gObjCompDeepestZIndex = ((int32_t)start[i] - (int32_t)&gObjCompsDense) / 16;
                foundDeepest = true;
            }
            for (int j = i + 1; j < 8; j++) {
                if (start[j]) {
                    end[i]->nextIndex = ((int32_t)start[j] - (int32_t)&gObjCompsDense) / 16;
                    start[j]->prevIndex = ((int32_t)end[i] - (int32_t)&gObjCompsDense) / 16;
                    i = j - 1; // skip empty layers
                    break;
                }
            }
        }
    }
}
