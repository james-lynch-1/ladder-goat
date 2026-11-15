#include "levels.h"

const LevelEntData level0EntArr[] = {
    { { 2, 0, 2 }, ENT_KEY, 0, { SOUTHEAST }, NULL },
    { { 7, 0, 1 }, ENT_WALKSWITCH, 0, { WALK_TOGGLE_FLAG, 2 }, walkSwitchCBEnableGoal },
    { { 6, 0, 5 }, ENT_GOAL, ENT_INACTIVE,{ 0, 2 }, walkSwitchCBChangeLevel }
};

const LevelData level0 = {
    .levelId = 0,
    .bgIndex = 0,
    .entArr = level0EntArr,
    .entArrLength = sizeof(level0EntArr) / sizeof(LevelEntData),
    .playerPos = { 3, 0, 1, SOUTHEAST },
    .yHeight = 3,
    .xWidth = 9,
    .zWidth = 8,
    .vp = { 32, 0, 0, INT32_MAX, -32, 0, 0, INT32_MAX, false },
    .clsn =
    {
        1,6,6,6,1,6,6,6,1,
        5,0,0,0,5,0,0,0,5,
        5,0,0,0,5,0,7,6,7,
        5,0,0,0,5,0,7,0,0,
        5,4,0,4,5,0,0,0,0,
        1,0,0,0,7,0,0,0,0,
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,

        1,0,0,0,1,0,0,0,1,
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,
        0,4,0,4,0,0,0,0,0,
        1,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,

        7,6,6,6,7,6,6,6,7,
        5,0,0,0,0,0,0,0,0,
        5,0,0,0,0,0,0,0,0,
        5,0,0,0,0,0,0,0,0,
        5,4,4,4,0,0,0,0,0,
        7,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,
    }
};
