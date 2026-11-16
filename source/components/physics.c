#include "component.h"

enum Direction getDirFromAngle(int angle) {
    return ((angle + 0x2000) / 0x4000) & 3;
}

void updatePhysics() {
    for (int i = 0; i < numComps(COMP_PHYSICS); i++) {
        updatePhysicsSingleEnt(&gPhysCompsDense[i]);
    }
}

void updatePhysicsSingleEnt(PhysicsComponent* phys) {
    PositionMini oldTilePos = getTilePosFromPos(phys->pos);
    Position newPos = phys->pos;
    RotationComponent* rot = getComponent(phys->header.entId, COMP_ROTATION);
    MemberComponent* member = getComponent(phys->header.entId, COMP_MEMBER);
    PhysicsComponent* parentPhys = member ? getComponent(member->groupIds[0], COMP_PHYSICS) : NULL;

    int targetAngle = phys->targetPos.dir * 0x4000;
    int angle = phys->angle;
    if (rot && angle != targetAngle) {
        int rotSrcEntId = getRotSrcEntId(member);
        RotationComponent* masterRot = getComponent(rotSrcEntId, COMP_ROTATION);
        if (masterRot) { // member ents derive rotation from (grand...)parent
            rot->mtx = masterRot->mtx;
            PhysicsComponent* masterPhys = getComponent(rotSrcEntId, COMP_PHYSICS);
            phys->angle = masterPhys->angle;
            gObjAffBuffer[gCompSetSparse[COMP_ROTATION][rot->header.entId]] =
                gObjAffBuffer[gCompSetSparse[COMP_ROTATION][masterRot->header.entId]];
        }
        else {
            if (targetAngle == 0 && angle > 0x8000) targetAngle += 0x10000;
            else if (targetAngle == SOUTHWEST * 0x4000 && angle == 0) angle += 0x10000;
            int goFast = ABS(targetAngle - angle) > 0x4000 ? 3 : 0;
            int diff = targetAngle - angle;
            angle += (SGN3(diff) << (11 + goFast));
            if (getDirFromAngle(angle) == phys->targetPos.dir &&
                phys->targetPos.dir != phys->tilePos.dir)
                phys->targetPos = phys->tilePos;
            else {
                phys->angle = angle;
                setInitMatrix(phys, rot);
                applyRotations(rot->header.entId);
            }
        }
    }

    if ((phys->targetPos.x << 16) * 16 != newPos.x.WORD ||
        (phys->targetPos.z << 16) * 16 != newPos.z.WORD) {
        if (phys->header.flags & PHYS_EDGE_MASK) { // edge of long ent derives pos from parent
            const int offs = phys->header.flags & PHYS_FRONT_EDGE_FLAG ? 1 : -1;
            newPos.x.WORD = parentPhys->pos.x.WORD + offs * (lu_cos(parentPhys->angle) << 4) * 16;
            newPos.z.WORD = parentPhys->pos.z.WORD - offs * (lu_sin(parentPhys->angle) << 4) * 16;
        }
        else {
            const Position targetPosToWorldPos = {
                .x = { (phys->targetPos.x << 16) * 16 },
                .z = { (phys->targetPos.z << 16) * 16 }
            };
            newPos.x.WORD += (targetPosToWorldPos.x.WORD - phys->pos.x.WORD) >> 3;
            if (ABS(newPos.x.WORD - targetPosToWorldPos.x.WORD) < 0x20000) newPos.x = targetPosToWorldPos.x;
            newPos.z.WORD += (targetPosToWorldPos.z.WORD - phys->pos.z.WORD) >> 3;
            if (ABS(newPos.z.WORD - targetPosToWorldPos.z.WORD) < 0x20000) newPos.z = targetPosToWorldPos.z;
        }
        if (isEqualPosMini(getTilePosFromPos(newPos), phys->targetPos) &&
            !isEqualPosMini(phys->tilePos, phys->targetPos)) {
            phys->targetPos = phys->tilePos;
            if (parentPhys) parentPhys->targetPos = parentPhys->tilePos;
            return;
        }
        if (!isEqualPos(phys->pos, newPos)) {
            phys->pos = newPos;
            ObjComponent* obj = getComponent(phys->header.entId, COMP_OBJ);
            if (obj) {
                updateObjUsingPtr(obj);
                if (!isEqualPosMini(oldTilePos, getTilePosFromPos(phys->pos)))
                    updateZDepth(obj);
            }
        }
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

void setTilePos(int entId, int x, int y, int z, enum Direction dir) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if (!phys) return;
    if (phys->header.flags & PHYS_SOLID_FLAG) setClsnVal(phys->tilePos, NULL);
    phys->tilePos.x = x;
    phys->tilePos.y = y;
    phys->tilePos.z = z;
    phys->tilePos.dir = dir;
    phys->targetPos = phys->tilePos;
    if (phys->header.flags & PHYS_SOLID_FLAG) setClsnVal(phys->tilePos, (ComponentHeader*)phys);
}

void setPosMini(PositionMini* tP, int x, int y, int z, enum Direction dir) {
    tP->x = x;
    tP->y = y;
    tP->z = z;
    tP->dir = dir;
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

PositionMini getTilePos(PhysicsComponent* phys) {
    PositionMini p = {
        (phys->pos.x.WORD / 16 + 0x8000) >> 16,
        (phys->pos.y.WORD / 16 + 0x8000) >> 16,
        (phys->pos.z.WORD / 16 + 0x8000) >> 16,
        getDirFromAngle(phys->angle)
    };
    return p;
}

PositionMini getTilePosFromPos(Position pos) {
    PositionMini p = {
        (pos.x.WORD / 16 + 0x8000) >> 16,
        (pos.y.WORD / 16 + 0x8000) >> 16,
        (pos.z.WORD / 16 + 0x8000) >> 16,
        0
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
    phys.targetPos = phys.tilePos;
    return (PhysicsComponent*)addComponentCustom(&phys, COMP_PHYSICS);
}

void removeComponentPhysics(int entId) {
    removeComponent(entId, COMP_PHYSICS);
}
