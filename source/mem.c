#include "mem.h"

int fetchSprite(const u16* tiles, int tilesLen) {
    // searching gSpriteAllocList to see if we are already using this sprite
    for (int i = 0; i < gNumSpritesAllocated; i++) {
        if (gSpriteAllocList[i].tileData == tiles) {
            gSpriteAllocList[i].numUsing++;
            return gSpriteAllocList[i].arrIndex;
        }
    }
    // couldn't find existing, so allocating a new sprite
    int index = allocateObj(tiles, tilesLen);
    if (index == -1) return -1; // no space for a new sprite
    SpriteAllocList alloc = { tiles, index, 1 };
    gSpriteAllocList[gNumSpritesAllocated++] = alloc;
    return index;
}

void stopUsingSprite(int sprAllocListIndex) {
    for (int i = 0; i < gNumSpritesAllocated; i++) {
        if (gSpriteAllocList[i].arrIndex == sprAllocListIndex) {
            gSpriteAllocList[i].numUsing--;
            if (gSpriteAllocList[i].numUsing == 0) {
                deallocateObj(gSpriteAllocList[i].arrIndex);
                gSpriteAllocList[i] = gSpriteAllocList[--gNumSpritesAllocated];
            }
            break;
        }
    }
}

int getObjAllocArrIndex(int sprAllocListIndex) {
    for (int i = 0; i < gNumSpritesAllocated; i++)
        if (gSpriteAllocList[i].arrIndex == sprAllocListIndex)
            return gSpriteAllocList[i].arrIndex;
    return -1;
}

int allocateObj(const u16* tiles, int tilesLen) {
    if ((tilesLen < 32) || (tilesLen > 2048) || (tilesLen & (tilesLen - 1)) != 0) return -1;
    int numBlocksReqd = tilesLen / 32;
    bool found = false;
    u32 index = 0;
    for (int i = 0; i < 1024; i += numBlocksReqd) {
        for (u32 j = i; j < i + numBlocksReqd; j++) {
            if (gObjAllocArr[j] != OBJ_SLOT_UNUSED) {
                found = false;
                break;
            }
            found = true;
        }
        if (found) {
            index = i;
            gObjAllocArr[i] = OBJ_SLOT_USED;
            for (++i; i < index + numBlocksReqd; i++)
                gObjAllocArr[i] = OBJ_SLOT_CONTINUE;
            memcpy32(MEM_VRAM_OBJ + (void*)(uint32_t)(index * 32), tiles, tilesLen / 4);
            return index;
        }
    }
    return -1;
}

void deallocateObj(int index) {
    int i = index;
    gObjAllocArr[i++] = OBJ_SLOT_UNUSED;
    while (gObjAllocArr[i] == OBJ_SLOT_CONTINUE)
        gObjAllocArr[i++] = OBJ_SLOT_UNUSED;
}

void loadBG(int sbbIndex, const u16* pal, int palLen, const u16* tiles, int tilesLen, const u16* map, int mapLen) {
    memcpy16(&pal_bg_bank[MAP_PAL], pal, palLen / sizeof(u16));
    memcpy32(&tile_mem[BG_MAP], tiles, tilesLen / sizeof(u32));
    int srcWInTiles = 32;
    for (int row = 0; row < 20; row++) {
        memcpy32(&se_mem[sbbIndex][row * SBB_WIDTH_T], &map[row * srcWInTiles], SBB_WIDTH_T / 2);
        for (int col = 0; col < srcWInTiles; col++) {
            se_mem[MAP_SBB][row * SBB_WIDTH_T + col] |= SE_PALBANK(MAP_PAL);
        }
    }
}

void lightenBgPalette(int amount) {
    for (int i = 0; i < 16; i++) {
        pal_bg_bank[MAP_PAL][i] += amount;
    }
}
