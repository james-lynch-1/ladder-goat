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

void updatePhysicsSingleEnt(PhysicsComponent* phys) {
    PositionMini oldTilePos = getTilePos(phys->header.entId);
    Position tilePosToPos;
    getPosFromTilePos(&tilePosToPos, phys->tilePos);
    bool changedPos = false;
    for (int i = 0; i < 3; i++) {
        SWord* posDim = (SWord*)&phys->pos + i;
        SWord* tPDim = (SWord*)&tilePosToPos + i;
        int diff = tPDim->HALF.HI - posDim->HALF.HI;
        if (diff) {
            changedPos = true;
            if (ABS(diff) >= 2)
                posDim->HALF.HI += SGN3(diff) * (1 + (ABS(diff) / 16));
            else posDim->WORD = tPDim->WORD;
        }
    }
    if (hasComponent(phys->header.entId, COMP_OBJ)) {
        if (changedPos) updateObj(phys->header.entId);
        if (!isEqualPosMini(oldTilePos, getTilePos(phys->header.entId)))
            updateZDepth(getComponent(phys->header.entId, COMP_OBJ));
    }
}

void setPhysPosAndDir(int entId, int x, int y, int z, enum Direction dir) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if (!phys) return;
    phys->pos.x.WORD = x;
    phys->pos.y.WORD = y;
    phys->pos.z.WORD = z;
    phys->angle = dir * 0x4000;
}

void setPosMiniAndDir(int entId, int x, int y, int z, enum Direction dir) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if (!phys) return;
    if (phys->header.flags & PHYS_SOLID_FLAG) setClsnVal(phys->tilePos, NULL);
    phys->tilePos.x = x;
    phys->tilePos.y = y;
    phys->tilePos.z = z;
    phys->tilePos.dir = dir;
    if (phys->header.flags & PHYS_SOLID_FLAG) setClsnVal(phys->tilePos, (ComponentHeader*)phys);
}

void taskMoveNESW(int entId, Task* task) {
    // PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    // if (!phys) return;
    // enum Direction dir = task->data;
    // int angle = dir * 0x4000;
    // phys->angle = angle;
    // if (task->timeRemaining == 16) {
    //     bool canMove = checkCollisionMove(phys, 1) == 0;
    //     if (!canMove) {
    //         task->timeRemaining = 1;
    //         return;
    //     }
    // }
    // moveEnt(entId, task, 1);
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

void getPosFromTilePos(Position* pos, PositionMini tP) {
    Position p = { { (tP.x << 16) * 16 },{ (tP.y << 16) * 16 },{ (tP.z << 16) * 16 } };
    *pos = p;
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
        .header = { entId, flags },
        .pos = { (SWord)posX, (SWord)posY, (SWord)posZ },
        .tilePos = { ((SWord)posX).HALF.HI / 16, ((SWord)posY).HALF.HI / 16, ((SWord)posZ).HALF.HI / 16, getDirFromAngle(angle) },
        .hitbox = { 0, 0, 0, 0, 0, 0 },
        .vec = { (SWord)vecX, (SWord)vecY, (SWord)vecZ },
        .actionCacheIndex = -1,
        .weight = weight,
        .angle = angle
    };
    return (PhysicsComponent*)addComponentCustom(&phys, COMP_PHYSICS);
}

void removeComponentPhysics(int entId) {
    removeComponent(entId, COMP_PHYSICS);
}
