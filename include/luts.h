#ifndef LUTS
#define LUTS

#include "global.h"

enum __attribute__ ((__packed__)) LutEnum {
    LUT_SMOOTH_STEP,
    LUT_SIN,
    LUT_SQRT,
    NUM_LUTS
};

extern const LutStruct gLuts[NUM_LUTS];

extern const s16 smoothStepLut[256];
extern const s16 sqrtLut[256];
extern const s16 yawScaleXLut[512];
extern const s16 yawShearYLut[512];

#endif
