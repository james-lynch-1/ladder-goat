#include "component.h"

enum Direction getDirFromAngle(int angle) {
    return angle / 0x4000;
}

void updatePhysics() {
    for (int i = 0; i < numComps(COMP_PHYSICS); i++) {
        PhysicsComponent* phys = &gPhysCompsDense[i];
        updatePhysicsSingleEnt(phys);
    }
}

inline void updatePhysicsSingleEnt(PhysicsComponent* phys) {
    PositionMini oldTilePos = getTilePos(phys->header.entId);
    phys->pos.x.WORD += phys->vec.x.WORD; //       |y
    phys->pos.y.WORD += phys->vec.y.WORD; //       .
    phys->pos.z.WORD += phys->vec.z.WORD; //     z/ \x
    if (phys->vec.x.WORD || phys->vec.y.WORD || phys->vec.z.WORD)
        updateObj(phys->header.entId);

    PositionMini newTilePos = getTilePos(phys->header.entId);
    if (!isEqualPosMini(oldTilePos, newTilePos))
        updateZDepth(getComponent(phys->header.entId, COMP_OBJ));
    phys->vec.x.WORD = phys->vec.y.WORD = phys->vec.z.WORD = 0;
}

void setPhysPosAndDir(int entId, int x, int y, int z, enum Direction dir) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if (!phys) return;
    phys->pos.x.WORD = x;
    phys->pos.y.WORD = y;
    phys->pos.z.WORD = z;
    phys->angle = dir * 0x4000;
}

void taskMoveNESW(int entId, Task* task) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if (!phys) return;
    enum Direction dir = task->data;
    int angle = dir * 0x4000;
    phys->angle = angle;
    if (task->timeRemaining == 16) {
        bool canMove = checkCollisionMove(phys, 1) == 0;
        if (!canMove) {
            task->timeRemaining = 1;
            return;
        }
    }
    moveEnt(entId, task, 1);
}

// utils

Vector3D decaySpeed(Vector3D vec, u32 rate) {
    vec.x.WORD = ((s64)rate * vec.x.WORD) >> 16;
    vec.y.WORD = ((s64)rate * vec.y.WORD) >> 16;
    return vec;
}

// https://en.m.wikipedia.org/wiki/Alpha_max_plus_beta_min_algorithm
u32 fastMagnitude(int x, int y) {
    u32 max = MAX(ABS(x), ABS(y));
    u32 min = MIN(ABS(x), ABS(y));
    return (((u64)max * 0b1111010111011111) >> 16) + ((u64)min * 0b0110010111011000 >> 16);
}

Vector3D addVec(Vector3D vec1, Vector3D vec2) {
    Vector3D vec = { { vec1.x.WORD + vec2.x.WORD },{ vec1.y.WORD + vec2.y.WORD } };
    return vec;
}

// uses lu_div tonc_math function to perform a reciprocal multiplication (0 ≤ x ≤ 255)
Vector3D divVec(Vector3D vec, u32 divisor) {
    vec.x.WORD *= lu_div(divisor);
    vec.y.WORD *= lu_div(divisor);
    vec.x.WORD >>= 16;
    vec.y.WORD >>= 16;
    return vec;
}

Vector3D normaliseVec(Vector3D vec) {
    u32 angle = ArcTan2(vec.x.WORD >> 12, -vec.y.WORD >> 12);
    Vector3D normVec = { { (lu_cos(angle)) << 4 },{ (-lu_sin(angle)) << 4 } };
    return normVec;
}

Vector3D scalarMultVec(Vector3D vec, int scalar) {
    Vector3D newVec = { { vec.x.WORD * scalar },{ vec.y.WORD * scalar } };
    return newVec;
}

PositionMini getTilePos(int entId) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    PositionMini p = {
        (phys->pos.x.WORD / 16 + 0x8000) >> 16,
        (phys->pos.y.WORD / 16 + 0x8000) >> 16,
        (phys->pos.z.WORD / 16 + 0x8000) >> 16,
    };
    return p;
}

PhysicsComponent* addComponentPhysics(int entId, u16 flags, int posX, int posY, int posZ, int vecX, int vecY, int vecZ, int weight, u16 angle) {
    PhysicsComponent phys = {
        { entId, flags },
        { (SWord)posX, (SWord)posY, (SWord)posZ },
        { 0, 0, 0, 0, 0, 0 },
        { (SWord)vecX, (SWord)vecY, (SWord)vecZ },
        weight,
        angle
    };
    return (PhysicsComponent*)addComponentCustom(&phys, COMP_PHYSICS);
}

void removeComponentPhysics(int entId) {
    removeComponent(entId, COMP_PHYSICS);
}
