#include "collision.h"
#include "assert.h"

//   y
//   ↑
//   o
//  ↙ ↘
// z    x

int getClsnValAtTilePos(PositionMini tilePos) {
    if (!in_range(tilePos.x, 0, gLevelData->xWidth) || !in_range(tilePos.y, 0, 3) || !in_range(tilePos.z, 0, gLevelData->zWidth))
        return 1;
    const int offs = getClsnOffsFromTilePos(tilePos);
    return gClsn[offs] != NULL;
}

int getClsnOffsFromTilePos(PositionMini tilePos) {
    return tilePos.y * gLevelXWidthZWidthProduct +
        tilePos.z * gLevelData->xWidth +
        tilePos.x;
}

void setClsnVal(PositionMini tP, ComponentHeader* comp) {
    int offs = getClsnOffsFromTilePos(tP);
    gClsn[offs] = comp;
}

bool isTilePosWithinBounds(PositionMini tP) {
    return (in_range(tP.x, 0, gLevelData->xWidth) &&
        in_range(tP.y, 0, gLevelData->yHeight) &&
        in_range(tP.z, 0, gLevelData->zWidth));
}

PositionMini getNextTilePos(PositionMini tP, int fwdBwdTribool) {
    static_assert(sizeof(PositionMini) == 4);
    int xTriBoolFromDirArr[4] = { fwdBwdTribool, 0, -fwdBwdTribool, 0 };
    int xTriBool = xTriBoolFromDirArr[tP.dir];
    int zTriBool = xTriBoolFromDirArr[3 - tP.dir];
    PositionMini tilePosBeingEntered = {
        .x = tP.x + xTriBool,
        .y = tP.y,
        .z = tP.z + zTriBool,
        .dir = tP.dir
    };
    return tilePosBeingEntered;
}

void setClsnDataActionCacheIndex(PositionMini tilePos, int idx) {
    gClsnData[getClsnOffsFromTilePos(tilePos)].actionCacheIndex = idx;
}

void addAction(PhysicsComponent* phys, int flags, PositionMini startPos, PositionMini endPos) {
    Action a = { .entId = phys->header.entId, .flags = flags, .startPos = startPos, .endPos = endPos };
    phys->actionCacheIndex = gNumActionsCached;
    setClsnDataActionCacheIndex(startPos, gNumActionsCached);
    gActionCache[gNumActionsCached] = a;
    addEnteredFlagsToClsnData(startPos, endPos);
    gNumActionsCached++;
}

bool addTurn(int entId, bool isCW) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if ((phys && phys->actionCacheIndex != -1) || gNumActionsCached >= MAX_MOVES_CACHED - 1) {
#ifdef DEBUG
        if (gNumActionsCached >= MAX_MOVES_CACHED - 1)
            log(CHAR, "too many moves!");
#endif
        return false;
    }

    GroupComponent* g = getComponent(entId, COMP_GROUP);
    if (g)
        for (int i = 0; i < g->numMembers; i++)
            if (!addTurn(g->memberIds[i], isCW)) return false;

    // if this ent is an edge-type ent, calculate the end pos and do addAction()s.
    // if this ent is the master (centre) ent, we don't move but we do change dir.
    int dirSign = isCW ? -1 : 1;

    // getting the final pos
    PositionMini endPos = phys->tilePos;
    endPos.dir = (endPos.dir + dirSign) & 3;

    PositionMini endPosIntermediate = endPos;
    if (phys->header.flags & PHYS_EDGE_MASK) {
        int frontPhysOffsArr[4][2] = { { -1, -1 },{ -1, 1 },{ 1, 1 },{ 1, -1 } };
        // SE-NE, NE-NW, NW-SW, SW-SE
        int backEdgeOffs = (phys->header.flags & PHYS_BACK_EDGE_FLAG) != 0 ? 2 : 0;
        int arrIdx = (phys->tilePos.dir + backEdgeOffs + isCW) % 4;
        endPos.x = phys->tilePos.x + frontPhysOffsArr[arrIdx][0];
        endPos.z = phys->tilePos.z + frontPhysOffsArr[arrIdx][1];
        if (phys->tilePos.dir & 1) // NE or SW - only change x
            endPosIntermediate.x = endPos.x;
        else
            endPosIntermediate.z = endPos.z;
        // add an intermediate action that is not run, but is checked and
        // whose success decides whether the final action is run
        addAction(phys, ACTION_INTERMEDIATE_FLAG | ACTION_INVALIDATE_PARENT_FLAG, phys->tilePos, endPosIntermediate);
        addAction(phys, ACTION_FINAL_FLAG | ACTION_INVALIDATE_PARENT_FLAG, endPosIntermediate, endPos);
    }
    else
        addAction(phys, ACTION_TURN_ONLY_FLAG, phys->tilePos, endPos);
    return true;
}

bool addMove(int entId, int fwdBwdTribool) {
    PhysicsComponent* phys = getComponent(entId, COMP_PHYSICS);
    if ((phys && phys->actionCacheIndex != -1) || gNumActionsCached == MAX_MOVES_CACHED) {
#ifdef DEBUG
        if (gNumActionsCached == MAX_MOVES_CACHED)
            log(CHAR, "too many moves!");
#endif
        return false;
    }

    GroupComponent* g = getComponent(entId, COMP_GROUP);
    if (g)
        for (int i = 0; i < g->numMembers; i++)
            addMove(g->memberIds[i], fwdBwdTribool);
    PositionMini endPos = getNextTilePos(phys->tilePos, fwdBwdTribool);
    addAction(phys, 0, phys->tilePos, endPos);
    return true;
}

void addEnteredFlagsToClsnData(PositionMini startPos, PositionMini endPos) {
    enum Direction dir;
    if (startPos.x < endPos.x) dir = SOUTHEAST;
    else if (startPos.x > endPos.x) dir = NORTHWEST;
    else if (startPos.z < endPos.z) dir = SOUTHWEST;
    else if (startPos.z > endPos.z) dir = NORTHEAST;
    else return;
    switch (dir) {
        case SOUTHEAST:
            gClsnData[getClsnOffsFromTilePos(endPos)].flags |= CLSNFLAG_ENTERED_XNEG;
            break;
        case NORTHEAST:
            gClsnData[getClsnOffsFromTilePos(endPos)].flags |= CLSNFLAG_ENTERED_ZPOS;
            break;
        case NORTHWEST:
            gClsnData[getClsnOffsFromTilePos(endPos)].flags |= CLSNFLAG_ENTERED_XPOS;
            break;
        case SOUTHWEST:
            gClsnData[getClsnOffsFromTilePos(endPos)].flags |= CLSNFLAG_ENTERED_ZNEG;
            break;
    }
}

void processActions() {
    for (int i = 0; i < gNumActionsCached; i++) {
        Action* a = &gActionCache[i];
        if (!(gClsnData[getClsnOffsFromTilePos(a->startPos)].flags & CLSNFLAG_ENTERED_MASK))
            processAction(a->startPos);
    }
    for (int i = 0; i < gNumActionsCached; i++) {
        Action* a = &gActionCache[i];
        PhysicsComponent* phys = getComponent(a->entId, COMP_PHYSICS);
        doMove(phys, a);
        phys->actionCacheIndex = -1;
    }
    for (int i = 0; i < gNumActionsCached; i++) {
        Action* a = &gActionCache[i];
        int startOffs = getClsnOffsFromTilePos(a->startPos);
        int endOffs = getClsnOffsFromTilePos(a->endPos);
        gClsnData[startOffs].flags = gClsnData[endOffs].flags = 0;
        gClsnData[startOffs].actionCacheIndex = gClsnData[endOffs].actionCacheIndex = -1;
    }
    gNumActionsCached = 0;
}

void doMove(PhysicsComponent* phys, Action* a) {
    if (!(a->flags & ACTION_INTERMEDIATE_FLAG))
        phys->targetPos = a->endPos;
    if (a->flags & ACTION_INVALID_FLAG) return;
    PositionMini old = phys->tilePos;
    // take start position directly from the phys comp in case this move had an intermediate move
    if (!(a->flags & ACTION_INTERMEDIATE_FLAG))
        phys->tilePos = a->endPos;
    if (a->flags & ACTION_TURN_ONLY_FLAG || !(phys->header.flags & PHYS_SOLID_FLAG))
        return;
    int oldOffs = getClsnOffsFromTilePos(old);
    if (!(a->flags & ACTION_INTERMEDIATE_FLAG))
        setClsnVal(a->endPos, (ComponentHeader*)phys);
    int enteredFlags = gClsnData[oldOffs].flags & CLSNFLAG_ENTERED_MASK;
    if (a->flags & ACTION_FINAL_FLAG ||
        (enteredFlags && isPowerOf2(enteredFlags))) return;
    setClsnVal(old, NULL);
}

void invalidateChildMoves(int entId) {
    GroupComponent* g = getComponent(entId, COMP_GROUP);
    if (!g) return;
    for (int i = 0; i < g->numMembers; i++) {
        invalidateChildMoves(g->memberIds[i]);
        PhysicsComponent* p = getComponent(g->memberIds[i], COMP_PHYSICS);
        if (p && (p->actionCacheIndex != -1))
            gActionCache[p->actionCacheIndex].flags |= ACTION_INVALID_FLAG;
    }
}

void invalidateSiblingAndParentMoves(int entId) {
    MemberComponent* m = getComponent(entId, COMP_MEMBER);
    if (!m) return;
    GroupComponent* g = getComponent(m->groupIds[0], COMP_GROUP);
    PhysicsComponent* groupPhys = getComponent(g->header.entId, COMP_PHYSICS);
    if (groupPhys && groupPhys->actionCacheIndex != -1)
        gActionCache[groupPhys->actionCacheIndex].flags |= ACTION_INVALID_FLAG;
    invalidateSiblingAndParentMoves(g->header.entId);
    for (int i = 0; i < g->numMembers; i++) {
        PhysicsComponent* p = getComponent(g->memberIds[i], COMP_PHYSICS);
        if (p && p->actionCacheIndex != -1)
            gActionCache[p->actionCacheIndex].flags |= ACTION_INVALID_FLAG;
    }
}

bool doFalseThings(Action* action) {
    if (action->flags & ACTION_INVALIDATE_PARENT_FLAG)
        invalidateSiblingAndParentMoves(action->entId);
    invalidateChildMoves(action->entId);
    return false;
}

bool processAction(PositionMini tP) {
    int offs = getClsnOffsFromTilePos(tP);
    if (gClsnData[offs].actionCacheIndex == -1) return false;

    Action* action = &gActionCache[gClsnData[offs].actionCacheIndex];
    enum ComponentType compType = getComponentType(gClsn[offs]);
    if (compType == COMP_CELL || gClsnData[offs].actionCacheIndex == -1)
        return doFalseThings(action);

    PhysicsComponent* phys = getComponent(action->entId, COMP_PHYSICS);
    if (action->flags & ACTION_INVALID_FLAG)
        return doFalseThings(action);

    if (gClsnData[offs].flags & CLSNFLAG_VISITED) {
        action->flags |= ACTION_INVALID_FLAG;
        if (action->flags & ACTION_INTERMEDIATE_FLAG) {
            Action* finalAction = &gActionCache[phys->actionCacheIndex];
            finalAction->flags |= ACTION_INVALID_FLAG;
        }
        return doFalseThings(action);
    }
    gClsnData[offs].flags |= CLSNFLAG_VISITED;

    if (!isTilePosWithinBounds(action->endPos)) {
        action->flags |= ACTION_INVALID_FLAG;
        return doFalseThings(action);
    }

    if (action->flags & ACTION_TURN_ONLY_FLAG)
        return false;

    int nextOffs = getClsnOffsFromTilePos(action->endPos);
    if ((gClsn[nextOffs] == NULL && gClsnData[nextOffs].actionCacheIndex == -1) ||
        processAction(action->endPos))
        return true;

    action->flags |= ACTION_INVALID_FLAG;
    if (action->flags & ACTION_INTERMEDIATE_FLAG) {
        Action* finalAction = &gActionCache[phys->actionCacheIndex];
        finalAction->flags |= ACTION_INVALID_FLAG;
    }
    return doFalseThings(action);
}

#define CW  -1
#define CCW 1
int checkCollisionTurn(PhysicsComponent* phys, int turnDir) {
    PositionMini tilePos = getTilePos(phys);
    bool isZFacing = (getDirFromAngle(phys->angle)) & 1;

    int tilePosXMinusOne = tilePos.x == 0 ? 0 : tilePos.x - 1;
    int tilePosZMinusOne = tilePos.z == 0 ? 0 : tilePos.z - 1;
    int tilePosXPlusOne = tilePos.x == MAP_WIDTH_X - 1 ? 15 : tilePos.x + 1;
    int tilePosZPlusOne = tilePos.z == MAP_WIDTH_Z - 1 ? 15 : tilePos.z + 1;
    PositionMini topLeftTilePos = { tilePosXMinusOne, tilePos.y, tilePosZMinusOne };
    PositionMini topRightTilePos = { tilePosXPlusOne, tilePos.y, tilePosZMinusOne };
    PositionMini btmLeftTilePos = { tilePosXMinusOne, tilePos.y, tilePosZPlusOne };
    PositionMini btmRightTilePos = { tilePosXPlusOne, tilePos.y, tilePosZPlusOne };
    PositionMini xPosPos = { tilePosXPlusOne, tilePos.y, tilePos.z };
    PositionMini xNegPos = { tilePosXMinusOne, tilePos.y, tilePos.z };
    PositionMini zPosPos = { tilePos.x, tilePos.y, tilePosZPlusOne };
    PositionMini zNegPos = { tilePos.x, tilePos.y, tilePosZMinusOne };
    int topLeft = getClsnValAtTilePos(topLeftTilePos);
    int topRight = getClsnValAtTilePos(topRightTilePos);
    int btmLeft = getClsnValAtTilePos(btmLeftTilePos);
    int btmRight = getClsnValAtTilePos(btmRightTilePos);
    int cardinalColl = getClsnValAtTilePos(xPosPos) | getClsnValAtTilePos(xNegPos) |
        getClsnValAtTilePos(zPosPos) | getClsnValAtTilePos(zNegPos);

    PositionMini slappablePositions[6] = { xPosPos, xNegPos, zPosPos, zNegPos };
    if ((isZFacing && turnDir == CCW) || (!isZFacing && turnDir == CW)) {
        int clsn = topLeft | btmRight | cardinalColl;
        if (clsn) return clsn;
        slappablePositions[4] = topLeftTilePos;
        slappablePositions[5] = btmRightTilePos;
    }
    else if ((isZFacing && turnDir == CW) || (!isZFacing && turnDir == CCW)) {
        int clsn = topRight | btmLeft | cardinalColl;
        if (clsn) return clsn;
        slappablePositions[4] = topRightTilePos;
        slappablePositions[5] = btmLeftTilePos;
    }

    // check slappables
    int numSlapped = 0;
    for (int i = 0; i < numComps(COMP_SLAPPABLE); i++) {
        SlappableComponent* slap = &gSlappableCompsDense[i];
        PositionMini slapTilePos = getTilePos((PhysicsComponent*)getComponent(slap->header.entId, COMP_PHYSICS));
        if (isEqualPosMini(slapTilePos, slappablePositions[0]) ||
            isEqualPosMini(slapTilePos, slappablePositions[1]) ||
            isEqualPosMini(slapTilePos, slappablePositions[2]) ||
            isEqualPosMini(slapTilePos, slappablePositions[3]) ||
            isEqualPosMini(slapTilePos, slappablePositions[4]) ||
            isEqualPosMini(slapTilePos, slappablePositions[5])) {
            slap->callback(slap->header.entId);
            numSlapped++;
        }
        if (numSlapped == 2)
            break;
    }

    // now check walkables
    int xOffsPos, xOffsNeg, zOffsPos, zOffsNeg;
    getXZOffsets(phys->angle, phys->hitbox, &xOffsPos, &xOffsNeg, &zOffsPos, &zOffsNeg);
    checkWalkables(phys->weight, tilePos, tilePos, xOffsPos, xOffsNeg, zOffsPos, zOffsNeg, turnDir);

    return false;
}
#undef CW
#undef CCW

void getXZOffsets(int angle, Hitbox hitbox, int* xOffsPos, int* xOffsNeg, int* zOffsPos, int* zOffsNeg) {
    bool isZFacing = getDirFromAngle(angle) & 1;
    bool axisIsPositiveArr[4] = { true, false, false, true };
    bool isFacingPositive = axisIsPositiveArr[getDirFromAngle(angle)];
    *xOffsPos = !isZFacing ? (isFacingPositive ? hitbox.fwd : hitbox.bwd) :
        (isFacingPositive ? hitbox.l : hitbox.r);
    *xOffsNeg = !isZFacing ? (isFacingPositive ? hitbox.bwd : hitbox.fwd) :
        (isFacingPositive ? hitbox.r : hitbox.l);
    *zOffsPos = isZFacing ? (isFacingPositive ? hitbox.fwd : hitbox.bwd) :
        (isFacingPositive ? hitbox.r : hitbox.l);
    *zOffsNeg = isZFacing ? (isFacingPositive ? hitbox.bwd : hitbox.fwd) :
        (isFacingPositive ? hitbox.l : hitbox.r);
}

void checkWalkables(int weight, PositionMini tilePos, PositionMini nextTilePos, int xOffsPos, int xOffsNeg, int zOffsPos, int zOffsNeg, int turnDir) {
    int offsArr[6] = { zOffsPos, xOffsPos, zOffsNeg, xOffsNeg, zOffsPos, xOffsPos };
    for (int i = 0; i < numComps(COMP_WALKABLE); i++) {
        WalkableComponent* walk = &gWalkableCompsDense[i];
        int walkId = walk->header.entId;
        PhysicsComponent* walkSwitchPhys = getComponent(walkId, COMP_PHYSICS);
        if (walkSwitchPhys && !isEntInactive(walkId)) {
            PositionMini switchTilePos = getTilePos(walkSwitchPhys);
            bool isOnSwitch = !isEntInactive(walkId) &&
                in_range(switchTilePos.x, tilePos.x - xOffsNeg, tilePos.x + xOffsPos + 1) &&
                in_range(switchTilePos.z, tilePos.z - zOffsNeg, tilePos.z + zOffsPos + 1);
            bool willBeOnSwitch = !isEntInactive(walkId) &&
                in_range(switchTilePos.x, nextTilePos.x - offsArr[3 + turnDir], nextTilePos.x + offsArr[1 + turnDir] + 1) &&
                in_range(switchTilePos.z, nextTilePos.z - offsArr[2 + turnDir], nextTilePos.z + offsArr[4 + turnDir] + 1);

            if (!isOnSwitch && willBeOnSwitch)
                checkWalkableSteppingOn(walk, weight);
            else if (isOnSwitch && !willBeOnSwitch)
                checkWalkableSteppingOn(walk, -weight);
        }
    }
}

void checkWalkableSteppingOn(WalkableComponent* walk, int weight) {
    walk->callback(walk->header.entId, weight);
    walk->currentWeight += weight;
    walk->currentWeight = clamp(walk->currentWeight, 0, INT32_MAX);
}
