// #include "component.h"

// ObjAffComponent* addComponentObjAff(s16 entId, u16 flags, bool isDblSize) {
//     ObjComponent* o = getComponent(entId, COMP_OBJ);
//     if (gNumCompsPerType[COMP_OBJ_AFF] >= MAX_OBJ_AFF_COMPONENTS || entId < 0 || !o) return NULL;
//     ObjAffComponent oA = { {entId, flags} };
//     getObj(o)->attr0 |= ATTR0_AFF | (isDblSize << 9);
//     getObj(o)->attr1 &= ~ATTR1_AFF_ID_MASK;
//     getObj(o)->attr1 |= ATTR1_AFF_ID(gNumCompsPerType[COMP_OBJ_AFF]);
//     ObjAffComponent* objAffComp = (ObjAffComponent*)addComponentCustom(&oA, COMP_OBJ_AFF);
//     o->header.flags &= ~ATTR0_MODE_MASK;
//     o->header.flags |= ATTR0_AFF | (isDblSize << 9);
//     return objAffComp;
// }

// void updateObjAffs() {
//     // for (int i = 0; i < numComps(COMP_OBJ_AFF); i++) {
//     //     ObjAffComponent* objAff = &gObjAffCompsDense[i];
//     //     OBJ_AFFINE* objAffBuf = getObjAff(objAff);
//     // }
// }

// void removeComponentObjAff(int entId) {
//     ObjAffComponent* objAff = getComponent(entId, COMP_OBJ_AFF);
//     if (!objAff) return;

//     // set the associated obj's attrs back to regular mode
//     ObjComponent* obj = getComponent(entId, COMP_OBJ);
//     if (obj) {
//         getObj(obj)->attr0 &= ~ATTR0_MODE_MASK;
//         obj->header.flags &= ~ATTR0_MODE_MASK;
//     }

//     int replacementEntId = removeComponent(entId, COMP_OBJ_AFF);
//     // if there's one objAff left, or it's the last objAff in the dense array, just clear its attrs
//     if (replacementEntId == entId) {
//         getObjAff(objAff)->pa = 0;
//         getObjAff(objAff)->pb = 0;
//         getObjAff(objAff)->pc = 0;
//         getObjAff(objAff)->pd = 0;
//         return;
//     }

//     // move the replacement objAff buffer attrs to their new location
//     ObjAffComponent* replacementObjAff = getComponent(replacementEntId, COMP_OBJ_AFF);
//     OBJ_AFFINE* replacementObjAffBufferPtr = getObjAff(replacementObjAff);
//     getObjAff(objAff)->pa = replacementObjAffBufferPtr->pa;
//     getObjAff(objAff)->pb = replacementObjAffBufferPtr->pb;
//     getObjAff(objAff)->pc = replacementObjAffBufferPtr->pc;
//     getObjAff(objAff)->pd = replacementObjAffBufferPtr->pd;

//     // set the replacement obj's aff id to this slot
//     ObjComponent* replacementObj = getComponent(replacementEntId, COMP_OBJ);
//     getObj(replacementObj)->attr1 &= ~ATTR1_AFF_ID_MASK;
//     getObj(replacementObj)->attr1 |= getObj(obj)->attr1 & ATTR1_AFF_ID_MASK;
//     getObj(obj)->attr1 &= ~ATTR1_AFF_ID_MASK;
// }
