#ifndef QUATERNION_H
#define QUATERNION_H

#include "transformation.h"

class Quaternion {
    public:
        float a;
        vec3 u;

        Quaternion(float a = 0.0f, float x = 0.0f, float y = 0.0f, float z = 0.0f);
        Quaternion(float a = 0.0f, vec3 u = vec3());

        Quaternion operator+=(const Quaternion &q);
        Quaternion operator+=(float f);
        Quaternion operator-=(const Quaternion &q);
        Quaternion operator-=(const float f);
        Quaternion operator*=(const float f);
        Quaternion operator*=(const Quaternion &q);
        Quaternion operator/=(const float f);

        float& operator[](int i);
};

Quaternion operator+(const Quaternion &q, float f);
Quaternion operator+(const Quaternion &q1, const Quaternion &q2);
Quaternion operator-(const Quaternion &q, float f);
Quaternion operator-(const Quaternion &q1, const Quaternion &q2);
Quaternion operator*(const Quaternion &q, float f);
Quaternion operator*(const Quaternion &q1, const Quaternion &q2);
Quaternion operator/(const Quaternion &q, float f);
bool operator==(const Quaternion &q1, const Quaternion &q2);
std::ostream& operator<<(std::ostream &os, const Quaternion &q);

float length(Quaternion q);
Quaternion conjugate(Quaternion q);
Quaternion inverse(Quaternion q);

Quaternion GetRotation(float theta = 0.0f, vec3 axis = vec3(0, 0, 0));
vec3 RotateVector(vec3 v, Quaternion q);
mat4 QuaternionToMatrix(Quaternion q);

#endif