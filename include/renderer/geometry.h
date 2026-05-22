#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vectors.h"

struct aabb{
    vec3 center;
    vec3 extents;
    aabb(vec3 p_center, vec3 p_extents);
    bool isinvolume(vec3 p);
    bool segmentintersect(vec3 a, vec3 b, vec3 &intersect);
};

struct plane{
    vec3 d;
    vec3 n;
    plane(vec3 point, vec3 normal);
    bool isinside(vec3 q);
    bool segmentintersect(vec3 a, vec3 b, vec3 &intersect);
};

struct line{
    vec3 a;
    vec3 b;
    line(vec3 p_a, vec3 p_b);
    vec3 getpoint(float t);
};

#endif