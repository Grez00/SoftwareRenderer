#ifndef VECTORS_H
#define VECTORS_H

#include <iostream>
#include <math.h>

// VECTOR 4

struct vec4 {
    float x, y, z, w;
    vec4(float pX = 0.0f, float pY = 0.0f, float pZ = 0.0f, float pW = 1.0f);
    vec4 operator+=(const vec4 &v2);
    vec4 operator+=(float f);
    vec4 operator-=(const vec4 &v2);
    vec4 operator-=(const float f);
    vec4 operator*=(const float f);
    vec4 operator*=(const vec4 &v2);
    vec4 operator/=(const float f);

    vec4 operator-();
    float& operator[](int i);
};

vec4 operator+(const vec4 &v, float f);
vec4 operator+(const vec4 &v1, const vec4 &v2);
vec4 operator-(const vec4 &v, float f);
vec4 operator-(const vec4 &v1, const vec4 &v2);
vec4 operator*(const vec4 &v, float f);
vec4 operator*(const vec4 &v1, const vec4 &v2);
vec4 operator/(const vec4 &v, float f);
bool operator==(const vec4 &v1, const vec4 &v2);
std::ostream& operator<<(std::ostream &os, const vec4 &v);

float dot(vec4 v1, vec4 v2);
vec4 cross(vec4 v1, vec4 v2);
float length(vec4 v);
vec4 normalize(vec4 v);
float distance(vec4 v1, vec4 v2);
vec4 abs(vec4 v);
vec4 lerp(vec4 v1, vec4 v2, float t);

// VECTOR 3

struct vec3 {
    float x, y, z;
    vec3(float pX = 0.0f, float pY = 0.0f, float pZ = 0.0f);
    vec3(const vec4 &v);
    vec3 operator+=(const vec3 &v2);
    vec3 operator+=(float f);
    vec3 operator-=(const vec3 &v2);
    vec3 operator-=(const float f);
    vec3 operator*=(const vec3 &v2);
    vec3 operator*=(const float f);
    vec3 operator/=(const float f);

    vec3 operator-();
    float& operator[](int i);
};

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
vec3 abs(vec3 v);
vec3 lerp(vec3 v1, vec3 v2, float t);

// VECTOR2

struct vec2 {
    float x, y;

    vec2(float pX = 0.0f, float pY = 0.0f);
    vec2(const vec3 &v);
    vec2(const vec4 &v);

    vec2 operator+=(const vec2 &v2);
    vec2 operator+=(float f);
    vec2 operator-=(const vec2 &v2);
    vec2 operator-=(const float f);
    vec2 operator*=(const float f);
    vec2 operator*=(const vec2 &v2);
    vec2 operator/=(const float f);

    vec2 operator-();
    float& operator[](int i);
};

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
vec2 abs(vec2 v);
vec2 lerp(vec2 v1, vec2 v2, float t);

// HELPERS

vec4 v3tov4(vec3 v, float w);

#endif