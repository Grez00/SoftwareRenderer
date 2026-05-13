#include "Renderer/vec4.h"
#include <math.h>

vec4::vec4(float pX, float pY, float pZ, float pW){
    x = pX;
    y = pY;
    z = pZ;
    w = pW;
}

vec4 vec4::operator+=(const vec4 &v2){
    *this = vec4(this->x + v2.x, this->y + v2.y, this->z + v2.z, this->w + v2.w);
    return *this;
}
vec4 vec4::operator+=(float f){
    *this = vec4(this->x + f, this->y + f, this->z + f, this->w + f);
    return *this;
}
vec4 vec4::operator-=(const vec4 &v2){
    *this = vec4(this->x - v2.x, this->y - v2.y, this->z - v2.z, this->w - v2.w);
    return *this;
}
vec4 vec4::operator-=(const float f){
    *this = vec4(this->x - f, this->y - f, this->z - f, this->w - f);
    return *this;
}
vec4 vec4::operator*=(const float f){
    *this = vec4(this->x * f, this->y * f, this->z * f, this->w * f);
    return *this;
}
vec4 vec4::operator*=(const vec4 &v2){
    *this = vec4(this->x * v2.x, this->y * v2.y, this->z * v2.z, this->w * v2.w);
    return *this;
}
vec4 vec4::operator/=(const float f){
    *this = vec4(this->x / f, this->y / f, this->z / f, this->w / f);
    return *this;
}

vec4 vec4::operator-(){
    *this = vec4(-this->x, -this->y, -this->z, -this->w);
    return *this;
}

vec4 operator+(const vec4 &v, float f){
    return vec4(v.x + f, v.y + f, v.z + f, v.w + f);
}
vec4 operator+(const vec4 &v1, const vec4 &v2){
    return vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}
vec4 operator-(const vec4 &v, float f){
    return vec4(v.x - f, v.y - f, v.z - f, v.w - f);
}
vec4 operator-(const vec4 &v1, const vec4 &v2){
    return vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}
vec4 operator*(const vec4 &v, float f){
    return vec4(v.x * f, v.y * f, v.z * f, v.w * f);
}
vec4 operator*(const vec4 &v1, const vec4 &v2){
    return vec4(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z, v1.w * v2.w);
}
vec4 operator/(const vec4 &v, float f){
    return vec4(v.x / f, v.y / f, v.z / f, v.w / f);
}
bool operator==(const vec4 &v1, const vec4 &v2){
    return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w);
}
std::ostream& operator<<(std::ostream &os, const vec4 &v){
    os << '(' << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ')';
    return os;
}

float dot(vec4 v1, vec4 v2){
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z) + (v1.w * v2.w); 
}

// TODO
vec4 cross(vec4 v1, vec4 v2){
    vec4 result = vec4((v1.y * v2.z) + (v1.z * v2.y), (v1.z * v2.x) + (v1.x * v2.z), (v1.x * v2.y) + (v1.y * v2.x));
    return result;
}

float length(vec4 v){
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) + pow(v.w, 2));
}

vec4 normalize(vec4 v){
    float magnitude = length(v);
    vec4 result = vec4(v.x / magnitude, v.y / magnitude, v.z / magnitude, v.w / magnitude);
    return result;
}

float distance(vec4 v1, vec4 v2){
    vec4 difference = v1 - v2;
    return length(difference);
}