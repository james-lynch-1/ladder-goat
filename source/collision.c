#include "collision.h"

//                                                      y
//                                                      ↑
//                                                      o
// each row is a row of 19 on the x axis               ↙ ↘
// each col is a col of 19 on the z axis              z    x

// collTiles:
// 0: nothing
// 1: white
// 2: purple

#define FWD 1
#define BWD -1
int checkCollisionMove(PhysicsComponent* phys, int dir) {
    int zTriBool = 0, xTriBool = 0;
    int colOffset = dir == 1 ? phys->hitbox.fwd : phys->hitbox.bwd;
    switch (phys->angle) {
        case 0:
            xTriBool = dir;
            break;
        case 0x4000:
            zTriBool = -dir;
            break;
        case 0x8000:
            xTriBool = -dir;
            break;
        case 0xC000:
            zTriBool = dir;
            break;
        default:
            return 0;
    }
    PositionMini tilePos = getTilePos(phys->header.entId);
    int row = (tilePos.x + xTriBool) + (tilePos.z + zTriBool) - 7;
    int col = (tilePos.x + xTriBool) - (tilePos.z + zTriBool) + 7;
    if (row <= 0 || row >= NUM_ROWS || col < 0 || col >= NUM_COLS)
        return 1;
    return gColl[tilePos.y].cell[tilePos.z + zTriBool + colOffset * zTriBool][tilePos.x + xTriBool + colOffset * xTriBool];
}
#undef FWD
#undef BWD

#define CW  -1
#define CCW 1
int checkCollisionTurn(PhysicsComponent* phys, int dir) {
    PositionMini tilePos = getTilePos(phys->header.entId);
    bool isZFacing = (phys->angle / 0x4000) & 1;
    int topLeft = gColl[tilePos.y].cell[tilePos.z - 1][tilePos.x - 1];
    int topRight = gColl[tilePos.y].cell[tilePos.z - 1][tilePos.x + 1];
    int btmLeft = gColl[tilePos.y].cell[tilePos.z + 1][tilePos.x - 1];
    int btmRight = gColl[tilePos.y].cell[tilePos.z + 1][tilePos.x + 1];
    int cardinalColl = gColl[tilePos.y].cell[tilePos.z - 1][tilePos.x] |
        gColl[tilePos.y].cell[tilePos.z + 1][tilePos.x] |
        gColl[tilePos.y].cell[tilePos.z][tilePos.x - 1] |
        gColl[tilePos.y].cell[tilePos.z][tilePos.x + 1];
    if ((isZFacing && dir == CCW) || (!isZFacing && dir == CW))
        return topLeft | btmRight | cardinalColl;
    if ((isZFacing && dir == CW) || (!isZFacing && dir == CCW))
        return topRight | btmLeft | cardinalColl;
    return false;
}
#undef CW
#undef CCW
