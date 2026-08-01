#include "global.h"

int gFrameCount = 0;
int gHitstunFrameCount = 0;
int gNumEnts = 0;
int gNumCustomPhysArchetypes = 0;
int gNumSpritesAllocated = 0;
int gNumEntsToDelete = 0;

int gNumTasks = 0;
EWRAM_BSS Task gTasks[MAX_TASKS];

s16 gPlayerId;
s16 gLadderId;
OBJ_ATTR gObjBuffer[128];
OBJ_AFFINE* gObjAffBuffer = (OBJ_AFFINE*)gObjBuffer;
enum ObjSlotEnum gObjAllocArr[1024];
SpriteAllocList gSpriteAllocList[MAX_ALLOC_SPRITES];
u8 gEntFlags[MAX_ENTS];
u8 gEntsToDelete[MAX_ENTS];

GameState gGameState;
u32 gFlags = 0;

// // events and listeners
int gNumListenersPerType[NUM_COMP_TYPES];
EWRAM_BSS EventListener gEventListeners[NUM_COMP_TYPES][MAX_EVENT_LISTENERS_PER_TYPE];

// components
EWRAM_BSS ObjComponent gObjCompsDense[MAX_OBJ_COMPONENTS];
EWRAM_BSS ObjAffComponent gObjAffCompsDense[MAX_OBJ_AFF_COMPONENTS];
EWRAM_BSS InputComponent gInputCompsDense[MAX_INPUT_COMPONENTS];
EWRAM_BSS PhysicsComponent gPhysCompsDense[MAX_PHYSICS_COMPONENTS];
EWRAM_BSS RotationComponent gRotCompsDense[MAX_ROTATION_COMPONENTS];
EWRAM_BSS TimerComponent gTimerCompsDense[MAX_TIMER_COMPONENTS];
EWRAM_BSS CounterComponent gCounterCompsDense[MAX_COUNTER_COMPONENTS];
EWRAM_BSS MemberComponent gMemberCompsDense[MAX_MEMBER_COMPONENTS];
EWRAM_BSS GroupComponent gGroupCompsDense[MAX_GROUP_COMPONENTS];
EWRAM_BSS TaskQueueComponent gTaskQueueCompsDense[MAX_TASK_QUEUE_COMPONENTS];

const uint32_t gCompTable[NUM_COMP_TYPES][4] = {
    {(uint32_t)&gObjCompsDense, sizeof(ObjComponent), MAX_OBJ_COMPONENTS, (uint32_t)removeComponentObj},
    {(uint32_t)&gObjAffCompsDense, sizeof(ObjAffComponent), MAX_OBJ_AFF_COMPONENTS, (uint32_t)removeComponentObjAff},
    {(uint32_t)&gInputCompsDense, sizeof(InputComponent), MAX_INPUT_COMPONENTS, (uint32_t)removeComponentInput},
    {(uint32_t)&gPhysCompsDense, sizeof(PhysicsComponent), MAX_PHYSICS_COMPONENTS, (uint32_t)removeComponentPhysics},
    {(uint32_t)&gRotCompsDense, sizeof(RotationComponent), MAX_ROTATION_COMPONENTS, (uint32_t)removeComponentRotation},
    {(uint32_t)&gTimerCompsDense, sizeof(TimerComponent), MAX_TIMER_COMPONENTS, (uint32_t)removeComponentTimer},
    {(uint32_t)&gCounterCompsDense, sizeof(CounterComponent), MAX_COUNTER_COMPONENTS, (uint32_t)removeComponentCounter},
    {(uint32_t)&gMemberCompsDense, sizeof(MemberComponent), MAX_MEMBER_COMPONENTS, (uint32_t)removeComponentMember},
    {(uint32_t)&gGroupCompsDense, sizeof(GroupComponent), MAX_GROUP_COMPONENTS, (uint32_t)removeComponentGroup},
    {(uint32_t)&gTaskQueueCompsDense, sizeof(TaskQueueComponent), MAX_TASK_QUEUE_COMPONENTS, (uint32_t)removeComponentTaskQueue},
};
int gNumCompsPerType[NUM_COMP_TYPES];

// sparse sets for components. each element is an index into the dense array
s16 gCompSetSparse[NUM_COMP_TYPES][MAX_ENTS];
