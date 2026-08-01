#include "component.h"

ComponentHeader* addComponentCustom(void* data, enum ComponentType componentType) {
    if (componentType >= NUM_COMP_TYPES ||
        gNumCompsPerType[componentType] >= maxComps(componentType))
        return NULL;
    ComponentHeader* dest = (void*)denseSetAddr(componentType) + compSize(componentType) * gNumCompsPerType[componentType];
    memcpy32((void*)dest, data, compSize(componentType) / 4);
    gCompSetSparse[componentType][((ComponentHeader*)data)->entId] = gNumCompsPerType[componentType]++;
    return dest;
}

int removeComponent(int entId, enum ComponentType componentType) {
    if (!hasComponent(entId, componentType)) return -1; // in case component doesn't exist
    int denseIndex = gCompSetSparse[componentType][entId];
    gCompSetSparse[componentType][entId] = -1;
    int replacementEntId = ((ComponentHeader*)(denseSetAddr(componentType) +
        compSize(componentType) * (gNumCompsPerType[componentType] - 1)))->entId;
    if (replacementEntId != entId) {
        void* dst = (void*)(denseSetAddr(componentType) + compSize(componentType) * denseIndex);
        void* src = (void*)(denseSetAddr(componentType) + compSize(componentType) * (gNumCompsPerType[componentType] - 1));
        memcpy32(dst, src, compSize(componentType) / 4);
        gCompSetSparse[componentType][replacementEntId] = denseIndex;
    }
    gNumCompsPerType[componentType]--;
    return replacementEntId;
}

bool hasComponent(s16 entId, enum ComponentType componentType) {
    int index = gCompSetSparse[componentType][entId];
    return index != -1;
}

void* getComponent(s16 entId, enum ComponentType componentType) {
    if (hasComponent(entId, componentType))
        return denseSetAddr(componentType) + compSize(componentType) * gCompSetSparse[componentType][entId];
    return NULL;
}

void* getComponentFromDenseIndex(int denseIndex, enum ComponentType componentType) {
    return denseSetAddr(componentType) + compSize(componentType) * denseIndex;
}

enum ComponentType getComponentType(ComponentHeader* compPtr) {
    for (int i = 0; i < NUM_COMP_TYPES; i++)
        if ((uint32_t)compPtr >= (uint32_t)denseSetAddr(i))
            return i;
    return -1;
}

void initialiseComponentArrays() {
    memset32(&gNumCompsPerType, 0, sizeof(gNumCompsPerType) / 4);
    for (int i = 0; i < NUM_COMP_TYPES; i++) {
        if (!denseSetAddr(i)) break;
        // memset32((void*)denseSetAddr[i], 0, compSize[i] / 4);
        // make the index from the dense set comp to the 
        for (int j = 0; j < maxComps(i); j++) {
            ComponentHeader* compHeader = denseSetAddr(i) + compSize(i) * j;
            compHeader->entId = -1;
        }
        // -1 means there is no comp of this type for this ent
        for (int j = 0; j < MAX_ENTS; j++)
            gCompSetSparse[i][j] = -1;
    }
}
