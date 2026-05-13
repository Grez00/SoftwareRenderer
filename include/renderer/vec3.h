#ifndef VEC3_H
#define VEC3_H

#include <iostream>

#include "vec4.h"

typedef struct vec3 {
    float x, y, z;
    vec3(float pX = 0.0f, float pY = 0.0f, float pZ = 0.0f);
    vec3(vec4 v);
    vec3 operator+=(const vec3 &v2);
    vec3 operator+=(float f);
    vec3 operator-=(const vec3 &v2);
    vec3 operator-=(const float f);
    vec3 operator*=(const vec3 &v2);
    vec3 operator*=(const float f);
    vec3 operator/=(const float f);

    vec3 operator-();
} vec3;

vec3 operator+(const vec3 &v, float f);
vec3 operator+(const vec3 &v1, const vec3 &v2);
vec3 operator-(const vec3 &v, float f);
vec3 operator-(const vec3 &v1, const vec3 &v2);
vec3 operator*(const vec3 &v1, const vec3 &v2);
vec3 operator*(const vec3 &v, float f);
vec3 operator/(const vec3 &v, float f);
bool operator==(const vec3 &v1, const vec3 &v2);
std::ostream& operator<<(std::ostream &os, const vec3 &v);

float dot(vec3 v1, vec3 v2);
vec3 cross(vec3 v1, vec3 v2);
float length(vec3 v);
vec3 normalize(vec3 v);
float distance(vec3 v1, vec3 v2);

#endif