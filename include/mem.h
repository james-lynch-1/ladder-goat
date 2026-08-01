#ifndef MEM
#define MEM

#include "global.h"

/** Returns index of sprite data in gObjAllocArr, allocating a new one if not found */
int fetchSprite(const u16* tiles, int tilesLen);

void stopUsingSprite(int sprAllocListIndex);

int getObjAllocArrIndex(int sprAllocListIndex);

/** https://www.gamedeveloper.com/programming/gameboy-advance-resource-management
 * 'OBJ Sprite Memory Management'
 * returns -1 if unsuccessful, returns index in allocation array if successful
 */
int allocateObj(const u16* tiles, int tilesLen);

void deallocateObj(int index);

void loadBG(int sbbIndex, const u16* pal, int palLen, const u16* tiles, int tilesLen, const u16* map, int mapLen);

void lightenBgPalette(int amount);

#endif
