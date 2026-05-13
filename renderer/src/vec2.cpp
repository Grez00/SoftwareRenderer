#include "Renderer/vec2.h"

vec2::vec2(float pX, float pY){
    x = pX;
    y = pY;
}
vec2::vec2(vec3 v){
    x = v.x;
    y = v.y;
}
vec2::vec2(vec4 v){
    x = v.x;
    y = v.y;
}

vec2 vec2::operator+=(const vec2 &v2){
    *this = vec2(this->x + v2.x, this->y + v2.y);
    return *this;
}
vec2 vec2::operator+=(float f){
    *this = vec2(this->x + f, this->y + f);
    return *this;
}
vec2 vec2::operator-=(const vec2 &v2){
    *this = vec2(this->x - v2.x, this->y - v2.y);
    return *this;
}
vec2 vec2::operator-=(const float f){
    *this = vec2(this->x - f, this->y - f);
    return *this;
}
vec2 vec2::operator*=(const float f){
    *this = vec2(this->x * f, this->y * f);
    return *this;
}
vec2 vec2::operator*=(const vec2 &v2){
    *this = vec2(this->x * v2.x, this->y * v2.y);
    return *this;
}
vec2 vec2::operator/=(const float f){
    *this = vec2(this->x / f, this->y / f);
    return *this;
}

vec2 vec2::operator-(){
    *this = vec2(-this->x, -this->y);
    return *this;
}

vec2 operator+(const vec2 &v, float f){
    return vec2(v.x + f, v.y + f);
}
vec2 operator+(const vec2 &v1, const vec2 &v2){
    return vec2(v1.x + v2.x, v1.y + v2.y);
}
vec2 operator-(const vec2 &v, float f){
    return vec2(v.x - f, v.y - f);
}
vec2 operator-(const vec2 &v1, const vec2 &v2){
    return vec2(v1.x - v2.x, v1.y - v2.y);
}
vec2 operator*(const vec2 &v, float f){
    return vec2(v.x * f, v.y * f);
}
vec2 operator*(const vec2 &v1, const vec2 &v2){
    return vec2(v1.x * v2.x, v1.y * v2.y);
}
vec2 operator/(const vec2 &v, float f){
    return vec2(v.x / f, v.y / f);
}
bool operator==(const vec2 &v1, const vec2 &v2){
    return (v1.x == v2.x && v1.y == v2.y);
}
std::ostream& operator<<(std::ostream &os, const vec2 &v){
    os << '(' << v.x << ", " << v.y << ')';
    return os;
}

float dot(vec2 v1, vec2 v2){
    return (v1.x * v2.x) + (v1.y * v2.y);
}

float length(vec2 v){
    return sqrt(pow(v.x, 2) + pow(v.y, 2));
}

vec2 normalize(vec2 v){
    float magnitude = length(v);
    vec2 result = vec2(v.x / magnitude, v.y / magnitude);
    return result;
}

float angle(vec2 v1, vec2 v2){
    return acos(dot(normalize(v1), normalize(v2)));
}

float distance(vec2 v1, vec2 v2){
    vec2 difference = v1 - v2;
    return length(difference);
}

float cross(vec2 v1, vec2 v2){
    return v1.x*v2.y - v1.y*v2.x;
}