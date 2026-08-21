#include "renderer/quaternion.h"

Quaternion::Quaternion(float a, float x, float y, float z){
    this->a = a;
    this->u = vec3(x, y, z);
}

Quaternion::Quaternion(float a, vec3 u){
    this->a = a;
    this->u = u;
}

Quaternion Quaternion::operator+=(const Quaternion &q){
    *this = Quaternion(this->a + q.a, this->u + q.u);
    return *this;
}
Quaternion Quaternion::operator+=(float f){
    *this = Quaternion(this->a + f, this->u + f);
    return *this;
}
Quaternion Quaternion::operator-=(const Quaternion &q){
    *this = Quaternion(this->a - q.a, this->u - q.u);
    return *this; 
}
Quaternion Quaternion::operator-=(const float f){
    *this = Quaternion(this->a - f, this->u - f);
    return *this;
}
Quaternion Quaternion::operator*=(const float f){
    *this = Quaternion(this->a * f, this->u * f);
    return *this;
}
Quaternion Quaternion::operator*=(const Quaternion &q){
    *this = Quaternion(this->a * q.a - dot(this->u, q.u), this->a * q.u + q.a * this->u + cross(this->u, q.u));
    return *this;
}
Quaternion Quaternion::operator/=(const float f){
    *this = Quaternion(this->a / f, this->u / f);
    return *this;
}

float& Quaternion::operator[](int i){
    i = i % 4;
    if (i == 0) return a;
    if (i == 1) return u.x;
    if (i == 2) return u.y;
    if (i == 3) return u.z;

    printf("Quaternion: Index out of bounds error\n");
    exit(-1);
}

Quaternion operator+(const Quaternion &q, float f){
    return Quaternion(q.a + f, q.u + f);
}
Quaternion operator+(const Quaternion &q1, const Quaternion &q2){
    return Quaternion(q1.a + q2.a, q1.u + q2.u);
}
Quaternion operator-(const Quaternion &q, float f){
    return Quaternion(q.a - f, q.u - f);
}
Quaternion operator-(const Quaternion &q1, const Quaternion &q2){
    return Quaternion(q1.a - q2.a, q1.u - q2.u);
}
Quaternion operator*(const Quaternion &q, float f){
    return Quaternion(q.a * f, q.u * f);
}
Quaternion operator*(const Quaternion &q1, const Quaternion &q2){
    return Quaternion(q1.a * q2.a - dot(q1.u, q2.u), q1.a * q2.u + q2.a * q1.u + cross(q1.u, q2.u));
}
Quaternion operator/(const Quaternion &q, float f){
    return Quaternion(q.a/f, q.u/f);
}
bool operator==(const Quaternion &q1, const Quaternion &q2){
    return (q1.a == q2.a && q1.u == q2.u);
}
std::ostream& operator<<(std::ostream &os, const Quaternion &q){
    os << q.a << " + " << q.u;
    return os;
}

Quaternion conjugate(Quaternion q){
    return Quaternion(q.a, -q.u);
}
float length(Quaternion q){
    return sqrt((q*conjugate(q)).a);
}
Quaternion inverse(Quaternion q){
    Quaternion con = conjugate(q);
    return con/(q*con).a;
}

Quaternion GetRotation(float theta, vec3 axis){
    return Quaternion(cos(theta/2), sin(theta/2) * axis);
}
vec3 RotateVector(vec3 v, Quaternion q){
    Quaternion v_quat = Quaternion(0, v);
    return (q * v_quat * conjugate(q)).u;
}
vec3 RotateVector(vec3 v, float theta, vec3 axis){
    return RotateVector(v, GetRotation(theta, axis));
}
// TODO
mat4 QuaternionToMatrix(Quaternion q){
    return mat4();
}