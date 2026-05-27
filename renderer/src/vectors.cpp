#include "renderer/vectors.h"

// VECTOR 2

vec2::vec2(float pX, float pY){
    x = pX;
    y = pY;
}
vec2::vec2(const vec3 &v){
    x = v.x;
    y = v.y;
}
vec2::vec2(const vec4 &v){
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
    return vec2(-this->x, -this->y);
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
    if (magnitude == 0) return v;
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

vec2 abs(vec2 v){
    return vec2(abs(v.x), abs(v.y));
}

// VECTOR 3

vec3::vec3(float pX, float pY, float pZ){
    x = pX;
    y = pY;
    z = pZ;
}
vec3::vec3(const vec4 &v){
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
    return vec3(-this->x, -this->y, -this->z);
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
    vec3 result = vec3((v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x));
    return result;
}

float length(vec3 v){
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2));
}

vec3 normalize(vec3 v){
    float magnitude = length(v);
    if (magnitude == 0) return v;
    vec3 result = vec3(v.x / magnitude, v.y / magnitude, v.z / magnitude);
    return result;
}

float distance(vec3 v1, vec3 v2){
    vec3 difference = v1 - v2;
    return length(difference);
}

vec3 abs(vec3 v){
    return vec3(abs(v.x), abs(v.y), abs(v.z));
}

// VECTOR 4

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
    return vec4(-this->x, -this->y, -this->z, -this->w);
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
    vec4 result = vec4((v1.y * v2.z) - (v1.z * v2.y), (v1.z * v2.x) - (v1.x * v2.z), (v1.x * v2.y) - (v1.y * v2.x));
    return result;
}

float length(vec4 v){
    return sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2) + pow(v.w, 2));
}

vec4 normalize(vec4 v){
    float magnitude = length(v);
    if (magnitude == 0) return v;
    vec4 result = vec4(v.x / magnitude, v.y / magnitude, v.z / magnitude, v.w / magnitude);
    return result;
}

float distance(vec4 v1, vec4 v2){
    vec4 difference = v1 - v2;
    return length(difference);
}

vec4 abs(vec4 v){
    return vec4(abs(v.x), abs(v.y), abs(v.z), abs(v.w));
}

// HELPERS

vec4 v3tov4(vec3 v, float w){
    return vec4(v.x, v.y, v.z, w);
}