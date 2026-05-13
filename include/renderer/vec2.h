#ifndef VEC2_H
#define VEC2_H

#include <iostream>
#include <math.h>

#include "vec3.h"
#include "vec4.h"

typedef struct vec2 {
    float x, y;

    vec2(float pX = 0.0f, float pY = 0.0f);
    vec2(vec3 v);
    vec2(vec4 v);

    vec2 operator+=(const vec2 &v2);
    vec2 operator+=(float f);
    vec2 operator-=(const vec2 &v2);
    vec2 operator-=(const float f);
    vec2 operator*=(const float f);
    vec2 operator*=(const vec2 &v2);
    vec2 operator/=(const float f);

    vec2 operator-();
} vec2;

vec2 operator+(const vec2 &v, float f);
vec2 operator+(const vec2 &v1, const vec2 &v2);
vec2 operator-(const vec2 &v, float f);
vec2 operator-(const vec2 &v1, const vec2 &v2);
vec2 operator*(const vec2 &v, float f);
vec2 operator*(const vec2 &v1, const vec2 &v2);
vec2 operator/(const vec2 &v, float f);
bool operator==(const vec2 &v1, const vec2 &v2);
std::ostream& operator<<(std::ostream &os, const vec2 &v);

float dot(vec2 v1, vec2 v2);
float angle(vec2 v1, vec2 v2);
float length(vec2 v);
vec2 normalize(vec2 v);
float distance(vec2 v1, vec2 v2);
float cross(vec2 v1, vec2 v2);

#endif