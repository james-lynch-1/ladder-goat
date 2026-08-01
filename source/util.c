#include "util.h"

u8 attr1SizesArr[4][4] = {
    {ATTR1_SIZE_8x8 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_8x16 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_8x32 >> ATTR1_SIZE_SHIFT, 0},
    {ATTR1_SIZE_16x8 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_16x16 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_16x32 >> ATTR1_SIZE_SHIFT, 0},
    {ATTR1_SIZE_32x8 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_32x16 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_32x32 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_32x64 >> ATTR1_SIZE_SHIFT},
    {0, 0, ATTR1_SIZE_64x32 >> ATTR1_SIZE_SHIFT, ATTR1_SIZE_64x64 >> ATTR1_SIZE_SHIFT}
};

int getAppropriateSpriteSize(int width, int height) {
    if (width > 64 || height > 64) return 0;
    int nextPowDimensions[2] = { max(8, nextPow2(width)), max(8, nextPow2(height)) };
    int arrIndexes[2] = { 0, 0 };
    for (int i = 0; i < 2; i++)
        for (int j = 3; j < 7; j++)
            if ((nextPowDimensions[i] >> j) & 1) {
                arrIndexes[i] = j - 3;
                break;
            }
    return (int)(attr1SizesArr[arrIndexes[0]][arrIndexes[1]]);
}

int getAppropriateSpriteShape(int width, int height) {
    if (!in_range(width, 0, 64) || !in_range(height, 0, 64)) return 0;
    int pow2Width = max(8, nextPow2(width));
    int pow2Height = max(8, nextPow2(height));
    if (pow2Width > pow2Height)
        return ATTR0_WIDE >> ATTR0_SHAPE_SHIFT;
    if (pow2Width == pow2Height)
        return ATTR0_SQUARE >> ATTR0_SHAPE_SHIFT;
    if (pow2Width < pow2Height)
        return ATTR0_TALL >> ATTR0_SHAPE_SHIFT;
    return ATTR0_SQUARE >> ATTR0_SHAPE_SHIFT;
}

void doNothing() {}

void logSomething() {
    log(CHAR, "something");
}

void reset() {
    log(CHAR, "resetting");
    SoftReset();
}

int nextPow2(int num) {
    int x = num - 1;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    return x + 1;
}

SWord setSWord(s16 hi, u16 lo) {
    SWord sWord = {.HALF.HI = hi, .HALF.LO = lo};
    return sWord;
}

SWord multSWord(SWord a, SWord b) {
    SWord result;
    result.WORD = ((s64)a.WORD * (s64)b.WORD) >> 16;
    return result;
}

void multMatrix2D(s16* result, s16* m1, s16* m2) {
    // s16 temp[4] = {result[0], result[1], result[2], result[3]};
    s16 temp[4];
    temp[0] = (m1[0] * m2[0] >> 8) + (m1[1] * m2[2] >> 8);
    temp[1] = (m1[0] * m2[1] >> 8) + (m1[1] * m2[3] >> 8);
    temp[2] = (m1[2] * m2[0] >> 8) + (m1[3] * m2[2] >> 8);
    temp[3] = (m1[2] * m2[1] >> 8) + (m1[3] * m2[3] >> 8);
    memcpy32(result, temp, sizeof(s16) * 4 / sizeof(u32));
    return;
}

Matrix3D* multMatrix3D(Matrix3D* result, Matrix3D* m1, Matrix3D* m2) {
    Matrix3D temp = {
        {multSWord(m1->m00, m2->m00).WORD + multSWord(m1->m01, m2->m10).WORD + multSWord(m1->m02, m2->m20).WORD},
        {multSWord(m1->m00, m2->m01).WORD + multSWord(m1->m01, m2->m11).WORD + multSWord(m1->m02, m2->m21).WORD},
        {multSWord(m1->m00, m2->m02).WORD + multSWord(m1->m01, m2->m12).WORD + multSWord(m1->m02, m2->m22).WORD},
        {multSWord(m1->m10, m2->m00).WORD + multSWord(m1->m11, m2->m10).WORD + multSWord(m1->m12, m2->m20).WORD},
        {multSWord(m1->m10, m2->m01).WORD + multSWord(m1->m11, m2->m11).WORD + multSWord(m1->m12, m2->m21).WORD},
        {multSWord(m1->m10, m2->m02).WORD + multSWord(m1->m11, m2->m12).WORD + multSWord(m1->m12, m2->m22).WORD},
        {multSWord(m1->m20, m2->m00).WORD + multSWord(m1->m21, m2->m10).WORD + multSWord(m1->m22, m2->m20).WORD},
        {multSWord(m1->m20, m2->m01).WORD + multSWord(m1->m21, m2->m11).WORD + multSWord(m1->m22, m2->m21).WORD},
        {multSWord(m1->m20, m2->m02).WORD + multSWord(m1->m21, m2->m12).WORD + multSWord(m1->m22, m2->m22).WORD}
    };
    memcpy32(result, &temp, sizeof(Matrix3D) / 4);
    return result;
}
