#ifndef MATRIX4_H
#define MATRIX4_H

#include <math.h>

#include "vectors.h"

typedef struct mat4 {
    float cells[4][4];
    mat4(float cells[4][4]);
    mat4(
        float aa, float ab, float ac, float ad, 
        float ba, float bb, float bc, float bd,
        float ca, float cb, float cc, float cd,
        float da, float db, float dc, float dd);
    mat4();
    static mat4 Identity();
} mat4;

mat4 operator+(const mat4 &m1, const mat4 &m2);
mat4 operator-(const mat4 &m1, const mat4 &m2);
mat4 operator*(const mat4 &m, float f);
mat4 operator*(const mat4 &m, int i);
mat4 operator*(const mat4 &m1, const mat4 &m2);
vec4 operator*(const mat4 &m, const vec4 &v);

mat4 transpose(mat4 m);
mat4 cofactor(mat4 m);
mat4 adjugate(mat4 m);
mat4 inverse(mat4 m);
float determinant(mat4 m);

#endif