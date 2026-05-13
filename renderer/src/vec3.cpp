#include "Renderer/vec3.h"
#include <math.h>

vec3::vec3(float pX, float pY, float pZ){
    x = pX;
    y = pY;
    z = pZ;
}
vec3::vec3(vec4 v){
    x = v.x;
    y = v.y;
    z = v.z;
}

vec3 vec3::operator+=(const vec3 &v2){
    *this = vec3(this->x + v2.x, this->y + v2.y, this->z + v2.z);
    return *this;
}
vec3 vec3::operator+=(float f){
    *this = vec3(this->x + f, this->y + f, this->z + f);
    return *this;
}
vec3 vec3::operator-=(const vec3 &v2){
    *this = vec3(this->x - v2.x, this->y - v2.y, this->z - v2.z);
    return *this;
}
vec3 vec3::operator-=(const float f){
    *this = vec3(this->x - f, this->y - f, this->z - f);
    return *this;
}
vec3 vec3::operator*=(const float f){
    *this = vec3(this->x * f, this->y * f, this->z * f);
    return *this;
}
vec3 vec3::operator*=(const vec3 &v2){
    *this = vec3(this->x * v2.x, this->y * v2.y, this->z * v2.z);
    return *this;
}
vec3 vec3::operator/=(const float f){
    *this = vec3(this->x / f, this->y / f, this->z / f);
    return *this;
}

vec3 vec3::operator-(){
    *this = vec3(-this->x, -this->y, -this->z);
    return *this;
}

vec3 operator+(const vec3 &v, float f){
    return vec3(v.x + f, v.y + f, v.z + f);
}
vec3 operator+(const vec3 &v1, const vec3 &v2){
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}
vec3 operator-(const vec3 &v, float f){
    return vec3(v.x - f, v.y - f, v.z - f);
}
vec3 operator-(const vec3 &v1, const vec3 &v2){
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}
vec3 operator*(const vec3 &v, float f){
    return vec3(v.x * f, v.y * f, v.z * f);
}
vec3 operator*(const vec3 &v1, const vec3 &v2){
    return vec3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
}
vec3 operator/(const vec3 &v, float f){
    return vec3(v.x / f, v.y / f, v.z / f);
}
bool operator==(const vec3 &v1, const vec3 &v2){
    return (v1.x == v2.x && v1.y == v2.y && v1.z == v2.z);
}
std::ostream& operator<<(std::ostream &os, const vec3 &v){
    os << '(' << v.x << ", " << v.y << ", " << v.z << ')';
    return os;
}

float dot(vec3 v1, vec3 v2){
    return (v1.x * v2.x) + (v1.y * v2.y) + (v1.z * v2.z);
}

vec3 cross(vec3 v1, vec3 v2){
    vec3 result = vec3((v1.y * v2.z) + (v1.z * v2.y), (v1.z * v2.x) + (v1.x * v2.z), (v1.x * v2.y) + (v1.y * v2.x));
    return result;
}

float length(vec3 v){
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

vec3 normalize(vec3 v){
    float magnitude = length(v);
    vec3 result = vec3(v.x / magnitude, v.y / magnitude, v.z / magnitude);
    return result;
}

float distance(vec3 v1, vec3 v2){
    vec3 difference = v1 - v2;
    return length(difference);
}