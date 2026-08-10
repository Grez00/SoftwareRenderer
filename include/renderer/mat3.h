#ifndef MATRIX3_H
#define MATRIX3_H

#include <math.h>

#include "mat4.h"
#include "vectors.h"

typedef struct mat3 {
    float cells[3][3];
    mat3(float cells[3][3]);
    mat3(
        float aa, float ab, float ac, 
        float ba, float bb, float bc,
        float ca, float cb, float cc
    );
    mat3(mat4 m);
    mat3();
    static mat3 Identity();
} mat3;

mat3 operator+(const mat3 &m1, const mat3 &m2);
mat3 operator-(const mat3 &m1, const mat3 &m2);
mat3 operator*(const mat3 &m, float f);
mat3 operator*(const mat3 &m, int i);
mat3 operator*(const mat3 &m1, const mat3 &m2);
vec3 operator*(const mat3 &m, const vec3 &v);
std::ostream& operator<<(std::ostream &os, const mat3 &m);

mat3 transpose(mat3 m);
mat3 cofactor(mat3 m);
mat3 adjugate(mat3 m);
mat3 inverse(mat3 m);
float determinant(mat3 m);

#endif