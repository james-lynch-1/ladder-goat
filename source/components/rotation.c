#include "component.h"

void updateRotComps() {
    for (int i = 0; i < numComps(COMP_ROTATION); i++) {
        RotationComponent* rot = &gRotCompsDense[i];

        Matrix3D initMatrix = rot->mtx;

        /* Camera. dimetric 2:1 pixel ratio projection */
        // Matrix3D rotAboutX = {
        //     {0x10000},{0}, {0},
        //     {0}, {lu_cos(0x1555) << 4}, {-lu_sin(0x1555) << 4},
        //     {0}, {lu_sin(0x1555) << 4}, {lu_cos(0x1555)}
        // };
        // Matrix3D rotAboutY = {
        //     {0xB505}, {0}, {0xB505},
        //     {0}, {0x10000}, {0},
        //     {-0xB505}, {0}, {0xB505}
        // };
        // above matrices multiplied to produce the matrix below
        Matrix3D worldMatrix = {
            {0xB505}, {0}, {0xB505},
            {0x592F}, {0XDEB0}, {-0X5930},
            {-0x9D8}, {0x7E20}, {0X9D7}
        };
        multMatrix3D(&initMatrix, &worldMatrix, &initMatrix);

        // Invert matrix for GBA screen-to-texture mapping
        int invDet = ((s64)0x10000 * (1 << 16)) / (
            (multSWord(initMatrix.m00, initMatrix.m11).WORD) -
            (multSWord(initMatrix.m01, initMatrix.m10).WORD)
            );
        s64 newMatrix[4] = { ((s64)invDet * initMatrix.m11.WORD) >> 16, ((s64)invDet * -initMatrix.m01.WORD) >> 16,
                             ((s64)invDet * -initMatrix.m10.WORD) >> 16, ((s64)invDet * initMatrix.m00.WORD) >> 16 };

        OBJ_AFFINE* oA = getObjAff((ObjAffComponent*)getComponent(rot->header.entId, COMP_OBJ_AFF));
        oA->pa = (newMatrix[0] >> 8) & 0xFFFF;
        oA->pb = (newMatrix[1] >> 8) & 0xFFFF;
        oA->pc = (newMatrix[2] >> 8) & 0xFFFF;
        oA->pd = (newMatrix[3] >> 8) & 0xFFFF;
    }
}

void makeRotation(Matrix3D* initMatrix, int angle, Vector3D* a) {
    SWord c = { lu_cos(angle) << 4 };
    SWord s = { lu_sin(angle) << 4 };
    SWord d = { 0x10000 - c.WORD };

    SWord x = multSWord(a->x, d);
    SWord y = multSWord(a->y, d);
    SWord z = multSWord(a->z, d);

    SWord axay = multSWord(x, a->y);
    SWord axaz = multSWord(x, a->z);
    SWord ayaz = multSWord(y, a->z);

    Matrix3D m = {
        {c.WORD + multSWord(x, a->x).WORD}, {axay.WORD - multSWord(s, a->z).WORD}, {axaz.WORD + multSWord(s, a->y).WORD},
        {axay.WORD + multSWord(s, a->z).WORD}, {c.WORD + multSWord(y, a->y).WORD}, {ayaz.WORD - multSWord(s, a->x).WORD},
        {axaz.WORD - multSWord(s, a->y).WORD}, {ayaz.WORD + multSWord(s, a->x).WORD}, {c.WORD + multSWord(z, a->z).WORD}
    };
    multMatrix3D(initMatrix, initMatrix, &m);
}

void multQtrn(Quaternion* result, Quaternion* q1, Quaternion* q2) {
    Quaternion temp = {
        {multSWord(q1->w, q2->x).WORD + multSWord(q1->x, q2->w).WORD + multSWord(q1->y, q2->z).WORD - multSWord(q1->z, q2->y).WORD},
        {multSWord(q1->w, q2->y).WORD - multSWord(q1->x, q2->z).WORD + multSWord(q1->y, q2->w).WORD + multSWord(q1->z, q2->x).WORD},
        {multSWord(q1->w, q2->z).WORD + multSWord(q1->x, q2->y).WORD - multSWord(q1->y, q2->x).WORD + multSWord(q1->z, q2->w).WORD},
        {multSWord(q1->w, q2->w).WORD - multSWord(q1->x, q2->x).WORD - multSWord(q1->y, q2->y).WORD - multSWord(q1->z, q2->z).WORD}
    };
    memcpy32(result, &temp, sizeof(Quaternion) / 4);
}

void getRotMatrixFromQtrn(Matrix3D* result, Quaternion* q) {
    SWord x2 = multSWord(q->x, q->x), y2 = multSWord(q->y, q->y), z2 = multSWord(q->z, q->z),
        xy = multSWord(q->x, q->y), xz = multSWord(q->x, q->z), yz = multSWord(q->y, q->z),
        wx = multSWord(q->w, q->x), wy = multSWord(q->w, q->y), wz = multSWord(q->w, q->z);
    Matrix3D temp = {
        {0x10000 - multSWord((SWord)0x20000, (SWord)(y2.WORD + z2.WORD)).WORD},
        {multSWord((SWord)0x20000, (SWord)(xy.WORD - wz.WORD)).WORD},
        {multSWord((SWord)0x20000, (SWord)(xz.WORD + wy.WORD)).WORD},
        {multSWord((SWord)0x20000, (SWord)(xy.WORD + wz.WORD)).WORD},
        {0x10000 - multSWord((SWord)0x20000, (SWord)(x2.WORD + z2.WORD)).WORD},
        {multSWord((SWord)0x20000, (SWord)(yz.WORD - wx.WORD)).WORD},
        {multSWord((SWord)0x20000, (SWord)(xz.WORD - wy.WORD)).WORD},
        {multSWord((SWord)0x20000, (SWord)(yz.WORD + wx.WORD)).WORD},
        {0x10000 - multSWord((SWord)0x20000, (SWord)(x2.WORD + y2.WORD)).WORD}
    };
    memcpy32(result, &temp, sizeof(Matrix3D) / 4);
}

void addVecs(Vector3D* result, const Vector3D* a, const Vector3D* b) {
    Vector3D temp = { {a->x.WORD + b->x.WORD}, {a->y.WORD + b->y.WORD}, {a->z.WORD + b->z.WORD} };
    memcpy32(result, &temp, sizeof(Vector3D) / 4);
}

Vector3D* multVecByScalar(Vector3D* result, const Vector3D* vec, const SWord scalar) {
    result->x = multSWord(vec->x, scalar);
    result->y = multSWord(vec->y, scalar);
    result->z = multSWord(vec->z, scalar);
    return result;
}

void rotateVec(Vector3D* result, const Vector3D* v, const Quaternion* q) {
    const Vector3D* b = (Vector3D*)q;
    int b2 = multSWord(b->x, b->x).WORD + multSWord(b->y, b->y).WORD + multSWord(b->z, b->z).WORD;

    Vector3D term2 = { b->x, b->y, b->z };
    multVecByScalar(&term2, &term2, multSWord(dot(v, b), (SWord)0x20000));

    Vector3D term3;
    cross(&term3, b, v);
    multVecByScalar(&term3, &term3, multSWord(q->w, (SWord)0x20000));

    multVecByScalar(result, v, (SWord)(multSWord(q->w, q->w).WORD - b2));
    addVecs(result, result, &term2);
    addVecs(result, result, &term3);
}

SWord dot(const Vector3D* a, const Vector3D* b) {
    SWord result = { multSWord(a->x, b->x).WORD + multSWord(a->y, b->y).WORD + multSWord(a->z, b->z).WORD };
    return result;
}

void cross(Vector3D* result, const Vector3D* a, const Vector3D* b) {
    Vector3D temp = { {multSWord(a->y, b->z).WORD - multSWord(a->z, b->y).WORD},
                      {multSWord(a->z, b->x).WORD - multSWord(a->x, b->z).WORD},
                      {multSWord(a->x, b->y).WORD - multSWord(a->y, b->x).WORD} };
    memcpy32(result, &temp, sizeof(Vector3D) / 4);
}

void addComponentRotation(int entId, u16 flags) {
    RotationComponent rot = { {entId, flags}, { {0x10000}, {0}, {0}, {0}, {0x10000}, {0}, {0}, {0}, {0x10000} } };
    addComponentCustom(&rot, COMP_ROTATION);
}

void removeComponentRotation(int entId) {
    removeComponent(entId, COMP_ROTATION);
}
