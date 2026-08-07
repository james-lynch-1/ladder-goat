#include "collision.h"

// each individual number is a cell with max value 16   y
// each group of 8 numbers is a col of 8 on the y axis  |
//                                                      o---x
// each row is a row of 16 on the x axis               /
// each col is a col of 16 on the z axis              z
CollisionMap colMap = { { // y, x, z
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,2,2,2,2,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
} };

#define FWD 1
#define BWD -1
int checkCollisionMove(PhysicsComponent* phys, int dir) {
    int zTriBool = 0, xTriBool = 0;
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
    int x = phys->pos.x.HALF.HI / 16;
    int z = phys->pos.z.HALF.HI / 16;
    int y = (phys->pos.y.HALF.HI - PLAYER_HEIGHT / 2) / 16;
    int row = (x + xTriBool * 2) - 7 + (z + zTriBool * 2);
    if (row < -1 || row > 21)
        return 1;
    return colMap[y][z + zTriBool * 2][x + xTriBool * 2];
}
#undef FWD
#undef BWD

#define CW  -1
#define CCW 1
int checkCollisionTurn(PhysicsComponent* phys, int dir) {
    int x = phys->pos.x.HALF.HI / 16;
    int z = phys->pos.z.HALF.HI / 16;
    int y = phys->pos.y.HALF.HI / 16;
    bool isZFacing = (phys->angle / 0x4000) & 1;
    int topLeft = colMap[y][z - 1][x - 1];
    int topRight = colMap[y][z - 1][x + 1];
    int btmLeft = colMap[y][z + 1][x - 1];
    int btmRight = colMap[y][z + 1][x + 1];
    int cardinalColl = colMap[y][z - 1][x] | colMap[y][z + 1][x] | colMap[y][z][x - 1] | colMap[y][z][x + 1];
    if ((isZFacing && dir == CCW) || (!isZFacing && dir == CW))
        return topLeft | btmRight | cardinalColl;
    if ((isZFacing && dir == CW) || (!isZFacing && dir == CCW))
        return topRight | btmLeft | cardinalColl;
    return false;
}
#undef CW
#undef CCW
