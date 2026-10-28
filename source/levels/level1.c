#include "levels.h"

const LevelEntData level1EntArr[] = {
    { {6, 0, 6}, ENT_KEY, 0, {SOUTHEAST}, NULL },
    { {0, 0, 0}, ENT_SLAPSWITCH, 0, {3, 0x300}, slapSwitchCBEnableGoal },
    { {1, 0, 4}, ENT_GOAL, ENT_INACTIVE, {0, 2}, walkSwitchCBChangeLevel }
};

const LevelData level1 = {
    .levelId = 1,
    .bgIndex = 0,
    .entArr = level1EntArr,
    .entArrLength = sizeof(level1EntArr) / sizeof(LevelEntData),
    .playerPos = { 6, 0, 6, SOUTHEAST },
    .yHeight = 3,
    .xWidth = 8,
    .zWidth = 8,
    .vp = { 24, 0, 0, INT32_MAX, -16, 0, 0, INT32_MAX, false },
    .clsn =
    {
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,1,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,

        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,

        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,
    }
};
