#ifndef UTIL
#define UTIL

#include "global.h"
#include "log.h"
#include "luts.h"

extern u8 attr1SizesArr[4][4];

/** Returns the appropriate sprite ATTR1 sprite size for width and height. */
int getAppropriateSpriteSize(int width, int height);

/** Returns the appropriate sprite ATTR0 sprite shape for width and height. */
int getAppropriateSpriteShape(int width, int height);

void doNothing();

void logSomething();

void reset();

// https://jameshfisher.com/2018/03/30/round-up-power-2/
int nextPow2(int num);

SWord setSWord(s16 hi, u16 lo);

SWord multSWord(SWord a, SWord b);

void multMatrix2D(s16* result, s16* m1, s16* m2);

Matrix3D* multMatrix3D(Matrix3D* result, Matrix3D* m1, Matrix3D* m2);

#endif
