#include "component.h"

void updatePhysics() {
    for (int i = 0; i < numComps(COMP_PHYSICS); i++) {
        PhysicsComponent* ent = &gPhysCompsDense[i];
        ent->pos.x.WORD += ent->vec.x.WORD; //       |y
        ent->pos.y.WORD += ent->vec.y.WORD; //       .
        ent->pos.y.WORD = clamp(ent->pos.y.WORD, 0, 96 << 16);
        ent->pos.z.WORD += ent->vec.z.WORD; //     z/ \x
        memset32(&ent->vec, 0, 3);
    }
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
    Vector3D vec = { {vec1.x.WORD + vec2.x.WORD}, {vec1.y.WORD + vec2.y.WORD} };
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
    Vector3D normVec = { {(lu_cos(angle)) << 4}, {(-lu_sin(angle)) << 4} };
    return normVec;
}

Vector3D scalarMultVec(Vector3D vec, int scalar) {
    Vector3D newVec = { {vec.x.WORD * scalar}, {vec.y.WORD * scalar} };
    return newVec;
}

PositionMini getTilePos(int entId) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    PositionMini p = {
        (phys->pos.x.WORD + 0x8000) >> 20,
        (phys->pos.y.WORD + 0x8000) >> 20,
        (phys->pos.z.WORD + 0x8000) >> 20
    };
    return p;
}

PhysicsComponent* addComponentPhysics(int entId, u16 flags, int posX, int posY, int posZ, PhysArchetype* arch, int vecX, int vecY, int vecZ, u16 angle) {
    PhysicsComponent phys = { {entId, flags},
        {(SWord)posX, (SWord)posY, (SWord)posZ},
        arch,
        {(SWord)vecX, (SWord)vecY, (SWord)vecZ},
        angle
    };
    return (PhysicsComponent*)addComponentCustom(&phys, COMP_PHYSICS);
}

void removeComponentPhysics(int entId) {
    removeComponent(entId, COMP_PHYSICS);
}
