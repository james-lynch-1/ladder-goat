#ifndef COMPONENT
#define COMPONENT

#include "global.h"
#include "mem.h"

#include "player.h"
#include "enemy.h"
#include "eventListener.h"

extern const TaskData gTaskTable[NUM_TASK_TYPES];

ComponentHeader* addComponentCustom(void* data, enum ComponentType componentType);

/** Moves last component in list to the slot of the one being removed.
 * Updates the corresponding entry in gCompSetSparse[componentType] to -1.
 * Decrements gNumCompsPerType[componentType].
 * Returns the entId of the replacement ent.
 */
int removeComponent(int entId, enum ComponentType componentType);

bool hasComponent(s16 entId, enum ComponentType componentType);

/** Gets the component of a given type on an entity */
void* getComponent(s16 entId, enum ComponentType componentType);

void* getComponentFromDenseIndex(int denseIndex, enum ComponentType componentType);

/** Get a component's type by checking if it is in the memory range of
 * a particular dense set.
 * As we iterate through gDenseSetAddresses, the addresses get smaller.
 * so, for a given component address, if it is greater than the current
 * dense set address, it is a component of that type.
 */
enum ComponentType getComponentType(ComponentHeader* compPtr);

void initialiseComponentArrays();

// COMPONENTS
// Obj

ObjComponent* addComponentObj(s16 entId, u16 flags, u16 attr0, u16 attr1, u16 attr2, int yOffset, int posSourceCompType);

/** Special component remove function for objs, as each obj component has
 * a pointer into the obj buffer. This means that if we just leave the data
 * there, we end up with junk. So we have to explicitly set the attrs to 0.
 */
void removeComponentObj(int entId);

/** Updates the pos based on the pos provided by corresponding comp of type posSourceCompType */
void updateObj(int entId);

int getObjZDepthPriority(ObjComponent* obj);

// Input

void addComponentInput(int entId, int flags, void (*inputHandler)(int entId));

void handleInputPlayer(int entId);

void handleInputLadder(int entId);

void handleInputGameover(int entId);

void updateInputComps();

void removeComponentInput(int entId);

Vector3D decaySpeed(Vector3D vec, u32 rate);

u32 fastMagnitude(int x, int y);

Vector3D addVec(Vector3D vec1, Vector3D vec2);

Vector3D divVec(Vector3D vec, u32 divisor);

Vector3D normaliseVec(Vector3D vec);

Vector3D scalarMultVec(Vector3D vec, int scalar);

PositionMini getTilePos(int entId);

// Physics

void updatePhysics();

PhysicsComponent* addComponentPhysics(int entId, u16 flags, int posX, int posY, int posZ, int vecX, int vecY, int vecZ, u16 angle);

void removeComponentPhysics(int entId);

// Rotation

void applyRotations(int entId);

void updateRotComps();

void makeRotation(Matrix3D* result, int angle, Vector3D* v);

void multQtrn(Quaternion* result, Quaternion* q1, Quaternion* q2);

void getRotMatrixFromQtrn(Matrix3D* result, Quaternion* q);

void addVecs(Vector3D* result, const Vector3D* a, const Vector3D* b);

Vector3D* multVecByScalar(Vector3D* result, const Vector3D* vec, const SWord scalar);

void rotateVec(Vector3D* result, const Vector3D* v, const Quaternion* q);

SWord dot(const Vector3D* a, const Vector3D* b);

void cross(Vector3D* result, const Vector3D* a, const Vector3D* b);

RotationComponent* addComponentRotation(int entId, u16 flags);

void removeComponentRotation(int entId);

// Timer

void updateTimers();

void addComponentTimer(s16 entId, u16 flags, u16 length, void(*callback));

void removeComponentTimer(int entId);

// Counter

CounterComponent* addComponentCounter(int entId, u16 flags, s16 curr, s16 max);

void removeComponentCounter(int entId);

CounterComponent* getCounterByFlags(int entId, u16 flags);

/** amount can be pos or neg */
void incrementCounter(CounterComponent* counter, int amount);

// Member

/** Helper function for creating a member component. Assumes group exists */
MemberComponent* addComponentMember(s16 entId, u16 flags, int groupId);

/** Removes a member component and also removes the reference to it in all its
 * parent group(s), if there are any.
 * We remove the id from the group's array by assigning the id at the last spot
 * in the array to the to-be-deleted id's spot and decrementing numMembers by 1.
 */
void removeComponentMember(int entId);

/** Removes a member from a group without deleting any components. */
void removeMemberCompFromGroup(int entIdMem, int entIdGroup);

void doGroupCallbacks(int entId);

// Group

/** Helper function for creating a group component */
GroupComponent* addComponentGroup(s16 entId, u16 flags, s16* memberIds, void (*fn)(struct MemberComponent_*, struct GroupComponent_*), int numMembers, int entKind);

void removeComponentGroup(int entId);

// Task Queue

TaskQueueComponent* addComponentTaskQueue(int entId, int flags);

void removeComponentTaskQueue(int entId);

void updateTaskQueues();

bool addTaskToQueue(int entId, int taskIndex, int data);

bool isTaskWithFlagsRunning(int taskFlags, int entId);

bool isTaskWithFlagsInQueue(int taskFlags, int entId, bool includeRunningTask);

bool isTaskQueueEmpty(TaskQueueComponent* tQ);

bool isTaskQueueFull(TaskQueueComponent* tQ);

// Cell

CellComponent* addComponentCell(int entId, int flags, int posX, int posY, int posZ);

void removeComponentCell(int entId);

#endif
