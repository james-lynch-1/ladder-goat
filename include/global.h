#ifndef GLOBAL
#define GLOBAL

// basically every other header file includes this one; it has the constants, globals and types

// #define DEBUG

#include "tonc.h"
#include <stdlib.h>
#include <string.h>

#include "type.h"
#include "component.h"
#include "constants.h"
#include "util.h"
#include "graphics.h"

extern int gFrameCount;
extern int gHitstunFrameCount;
extern int gNumEnts;
extern int gNumCustomPhysArchetypes;
extern int gNumSpritesAllocated;
extern int gNumEntsToDelete;

extern int gNumTasks;
extern Task gTasks[MAX_TASKS];

extern s16 gPlayerId;
extern s16 gLadderId;

extern OBJ_ATTR gObjBuffer[128];
extern OBJ_AFFINE* gObjAffBuffer;
extern enum ObjSlotEnum gObjAllocArr[1024];
extern SpriteAllocList gSpriteAllocList[MAX_ALLOC_SPRITES];
extern u8 gEntFlags[MAX_ENTS];
extern u8 gEntsToDelete[MAX_ENTS];

extern GameState gGameState;
extern u32 gFlags;

// // events and listeners
extern int gNumListenersPerType[NUM_COMP_TYPES];
extern EventListener gEventListeners[NUM_COMP_TYPES][MAX_EVENT_LISTENERS_PER_TYPE];

// components
extern ObjComponent gObjCompsDense[MAX_OBJ_COMPONENTS];
extern ObjAffComponent gObjAffCompsDense[MAX_OBJ_AFF_COMPONENTS];
extern InputComponent gInputCompsDense[MAX_INPUT_COMPONENTS];
extern PhysicsComponent gPhysCompsDense[MAX_PHYSICS_COMPONENTS];
extern RotationComponent gRotCompsDense[MAX_ROTATION_COMPONENTS];
extern TimerComponent gTimerCompsDense[MAX_TIMER_COMPONENTS];
extern CounterComponent gCounterCompsDense[MAX_COUNTER_COMPONENTS];
extern MemberComponent gMemberCompsDense[MAX_MEMBER_COMPONENTS];
extern GroupComponent gGroupCompsDense [MAX_GROUP_COMPONENTS];
extern TaskQueueComponent gTaskQueueCompsDense[MAX_TASK_QUEUE_COMPONENTS];

extern int gNumCompsPerType[NUM_COMP_TYPES];

/** Table with addresses of comp dense sets, sizes of comps, max comps per type and comp removers */
enum gCompTableIndexes { COMP_DSET_ADDRESSES, COMP_SIZES, COMP_MAX_PER_TYPE, COMP_REMOVERS };

#define denseSetAddr(x) (void*)gCompTable[x][COMP_DSET_ADDRESSES]
#define compSize(x)     (int)gCompTable[x][COMP_SIZES]
#define maxComps(x)     (int)gCompTable[x][COMP_MAX_PER_TYPE]
#define numComps(x)     gNumCompsPerType[x]
#define getObj(x)       (&gObjBuffer[x->objIndex]) // x is an ObjComponent pointer
#define getObjAff(x)    (&gObjAffBuffer[(getObj(((ObjComponent*)getComponent((x)->header.entId, COMP_OBJ)))->attr1 & ATTR1_AFF_ID_MASK) >> ATTR1_AFF_ID_SHIFT]) // x is an ObjAffComponent pointer

extern const uint32_t gCompTable[NUM_COMP_TYPES][4];

// sparse sets for components. each element is an index into the dense array
extern s16 gCompSetSparse[NUM_COMP_TYPES][MAX_ENTS];

extern void setGameState(enum GameState state);

#endif
