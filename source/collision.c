#include "collision.h"

//                                                      y
//                                                      ↑
//                                                      o
// each row is a row of 19 on the x axis               ↙ ↘
// each col is a col of 19 on the z axis              z    x

int getClsnVal(CellComponent* cell) {
    if (cell)
        return cell->clsnVal;
    return 0;
}

int getClsnValAtTilePos(PositionMini tilePos) {
    CellComponent* cell = gColl[tilePos.y][tilePos.z][tilePos.x];
    return getClsnVal(cell);
}

#define FWD 1
#define BWD -1
int checkCollisionMove(PhysicsComponent* phys, int fwdBwdTribool) {
    int xTriBoolFromDirArr[4] = { fwdBwdTribool, 0, -fwdBwdTribool, 0 };
    int xTriBool = xTriBoolFromDirArr[getDirFromAngle(phys->angle)];
    int zTriBool = xTriBoolFromDirArr[3 - getDirFromAngle(phys->angle)];
    PositionMini tilePos = getTilePos(phys->header.entId);
    int row = (tilePos.x + xTriBool) + (tilePos.z + zTriBool) - 7;
    int col = (tilePos.x + xTriBool) - (tilePos.z + zTriBool) + 7;
    if (row <= 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS)
        return 1;

    // translate from hitbox-space offsets to world-space offsets
    int xOffsPos, xOffsNeg, zOffsPos, zOffsNeg;
    getXZOffsets(phys->angle, phys->hitbox, &xOffsPos, &xOffsNeg, &zOffsPos, &zOffsNeg);
    int xTriArr[3] = { -xOffsNeg, 0, xOffsPos };
    int zTriArr[3] = { -zOffsNeg, 0, zOffsPos };
    PositionMini nextTilePos = { tilePos.x + xTriBool, tilePos.y, tilePos.z + zTriBool };
    int clsn = getClsnVal(gColl[tilePos.y][nextTilePos.z + zTriArr[zTriBool + 1]][nextTilePos.x + xTriArr[xTriBool + 1]]);
    if (clsn) return clsn;

    checkWalkables(phys->weight, tilePos, nextTilePos, xOffsPos, xOffsNeg, zOffsPos, zOffsNeg, 0);

    return clsn;
}
#undef FWD
#undef BWD

#define CW  -1
#define CCW 1
int checkCollisionTurn(PhysicsComponent* phys, int turnDir) {
    PositionMini tilePos = getTilePos(phys->header.entId);
    bool isZFacing = (getDirFromAngle(phys->angle)) & 1;

    PositionMini topLeftTilePos = { tilePos.x - 1, tilePos.y, tilePos.z - 1 };
    PositionMini topRightTilePos = { tilePos.x + 1, tilePos.y, tilePos.z - 1 };
    PositionMini btmLeftTilePos = { tilePos.x - 1, tilePos.y, tilePos.z + 1 };
    PositionMini btmRightTilePos = { tilePos.x + 1, tilePos.y, tilePos.z + 1 };
    int topLeft = getClsnValAtTilePos(topLeftTilePos);
    int topRight = getClsnValAtTilePos(topRightTilePos);
    int btmLeft = getClsnValAtTilePos(btmLeftTilePos);
    int btmRight = getClsnValAtTilePos(btmRightTilePos);
    int cardinalColl = getClsnVal(gColl[tilePos.y][tilePos.z - 1][tilePos.x]) |
        getClsnVal(gColl[tilePos.y][tilePos.z + 1][tilePos.x]) |
        getClsnVal(gColl[tilePos.y][tilePos.z][tilePos.x - 1]) |
        getClsnVal(gColl[tilePos.y][tilePos.z][tilePos.x + 1]);

    PositionMini slappablePositions[2];
    if ((isZFacing && turnDir == CCW) || (!isZFacing && turnDir == CW)) {
        int clsn = topLeft | btmRight | cardinalColl;
        if (clsn) return clsn;
        slappablePositions[0] = topLeftTilePos;
        slappablePositions[1] = btmRightTilePos;
    }
    else if ((isZFacing && turnDir == CW) || (!isZFacing && turnDir == CCW)) {
        int clsn = topRight | btmLeft | cardinalColl;
        if (clsn) return clsn;
        slappablePositions[0] = topRightTilePos;
        slappablePositions[1] = btmLeftTilePos;
    }

    // check slappables
    int numSlapped = 0;
    for (int i = 0; i < numComps(COMP_SLAPPABLE); i++) {
        SlappableComponent* slap = &gSlappableCompsDense[i];
        PositionMini slapTilePos = getTilePos(slap->header.entId);
        if (isEqualPosMini(slapTilePos, slappablePositions[0]) ||
            isEqualPosMini(slapTilePos, slappablePositions[1])) {
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
            PositionMini switchTilePos = getTilePos(walkId);
            bool isOnSwitch = !isEntInactive(walkId) &&
                in_range(switchTilePos.x, tilePos.x - xOffsNeg, tilePos.x + xOffsPos + 1) &&
                in_range(switchTilePos.z, tilePos.z - zOffsNeg, tilePos.z + zOffsPos + 1);
            bool willBeOnSwitch = !isEntInactive(walkId) &&
                in_range(switchTilePos.x, nextTilePos.x - offsArr[3 + turnDir], nextTilePos.x + offsArr[1 + turnDir] + 1) &&
                in_range(switchTilePos.z, nextTilePos.z - offsArr[2 + turnDir], nextTilePos.z + offsArr[4 + turnDir] + 1);

            if (!isOnSwitch && willBeOnSwitch)
                checkWalkableSteppingOn(walk, weight);

            if (isOnSwitch && !willBeOnSwitch)
                checkWalkableSteppingOff(walk, weight);
        }
    }
}

void checkWalkableSteppingOn(WalkableComponent* walk, int weight) {
    walk->weightToAdd = weight;
    walk->callback(walk->header.entId);
    ObjComponent* switchObj = getComponent(walk->header.entId, COMP_OBJ);
    if (walk->currentWeight == 0)
        changePalette(
            switchObj,
            ((switchObj->attr2 & ATTR2_PALBANK_MASK) >> ATTR2_PALBANK_SHIFT) == PAL_ORANGE ?
            PAL_PURPLE : PAL_ORANGE);
    walk->currentWeight += walk->weightToAdd;
    walk->weightToAdd = 0;
}

void checkWalkableSteppingOff(WalkableComponent* walk, int weight) {
    walk->weightToAdd = -weight;
    walk->callback(walk->header.entId);
    walk->currentWeight += walk->weightToAdd;
    walk->weightToAdd = 0;
}
