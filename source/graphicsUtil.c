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
    gSpriteCellStartingId = reserveEntSlot();
    addComponentCell(gSpriteCellStartingId, 0, 0, 0, 0);
    addComponentObj(gSpriteCellStartingId, 0, 0, 0, 0, 0, COMP_CELL);
    for (int i = 1; i < 50; i++) {
        int entId = reserveEntSlot();
        addComponentCell(entId, 0, 0, 0, 0);
        addComponentObj(entId, 0,
            ATTR0_SQUARE,
            ATTR1_SIZE_32,
            ATTR2_PALBANK(2),
            0, COMP_CELL);
    }
    Position pos = { {0}, {0}, {0} };
    if (hasComponent(gLadderId, COMP_PHYSICS))
        pos = ((PhysicsComponent*)getComponent(gLadderId, COMP_PHYSICS))->pos;
    drawSpriteCells(pos);
}

void drawSpriteCells(Position pos) { // call this when we change tiles and when we init
    PositionMini tilePos = { pos.x.HALF.HI / 16, pos.y.HALF.HI / 16, pos.z.HALF.HI / 16 };
    int screenTileX = tilePos.x - tilePos.z + 7;
    bool isScreenTileXEven = !(screenTileX & 1);
    PositionMini topTile = { (screenTileX + 1) / 2, tilePos.y, 7 - screenTileX / 2 };
    topTile.x -= 1;
    topTile.z += isScreenTileXEven;
    // if screenTileX is even, do 3 row first. If odd, do 2 row first.
    for (int row = 0; row < 10; row++) {
        PositionMini tile = topTile;
        for (int t = 0; t < 5; t++) {
            int id = gSpriteCellStartingId + row * 5 + t;
            CellComponent* c = getComponent(id, COMP_CELL);
            ObjComponent* o = getComponent(id, COMP_OBJ);
            o->zDepth = row * 10 + t * 2; // TODO: take into account y
            // reset every time so it's a solid line of nextIndexes, all spritecells
            // effectively remove the player for now
            o->nextIndex = ((int32_t)((ComponentHeader*)getComponent(id + 1, COMP_OBJ)) -
                (int32_t)&gObjCompsDense) / 16;
            o->attr0 &= ~ATTR0_MODE_MASK;
            int colVal = colMap[tile.y][tile.z][tile.x];
            int sprIndex = o->attr2 & ~ATTR2_ID_MASK;
            c->pos.x.WORD = (tile.x * 16 - 8) << 16;
            c->pos.y.WORD = (tile.y * 16) << 16;
            c->pos.z.WORD = (tile.z * 16 - 8) << 16;
            o->attr2 = o->attr2 & ~ATTR2_ID_MASK;
            if (colVal == 0) {
                if (sprIndex != 0) stopUsingSprite(sprIndex);
                o->header.flags |= ATTR0_HIDE;
            }
            else {
                o->attr2 |= ATTR2_ID(fetchSprite(gCollTileToSpriteMap[colVal], 512)) & ATTR2_ID_MASK;
                o->header.flags &= ~ATTR0_MODE_MASK;
            }
            tile.x += 1; tile.z -= 1;
            // t=1 (odd) or t=2 (even): -2x, +3z
            if (t == (1 + isScreenTileXEven)) {
                tile.x -= 2;
                tile.z += 3;
            }
        }
        topTile.x += 1;
        topTile.z += 1;
    }
}
