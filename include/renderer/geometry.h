#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "vectors.h"
#include "camera.h"

struct aabb{
    vec3 center;
    vec3 extents;
    aabb(vec3 p_center, vec3 p_extents);
    bool isinside(vec3 p);
};

struct plane{
    vec3 p;
    vec3 n;
    float d;
    plane(vec3 point, vec3 normal);
    plane();
    float distance(vec3 q);
    bool isinside(vec3 q);
    vec3 projectpoint(vec3 q);
};

struct line{
    vec3 a;
    vec3 b;
    line(vec3 p_a, vec3 p_b);
    vec3 getpoint(float t);
};

struct sphere{
    vec3 c;
    float r;
    sphere(vec3 p_c, float p_r);
    bool isinside(vec3 q);
};

struct frustum{
    plane planes[6];
    frustum(plane *p_planes);
    frustum(Camera cam);
    bool isinside(vec3 q);
};

bool AABBSegmentIntersect(aabb a, line l, vec3 &intersect);
bool AABBSegmentIntersect(aabb box, vec3 a, vec3 b, vec3 &intersect);

bool PlaneSegmentIntersect(plane p, line l, vec3 &intersect);
bool PlaneSegmentIntersect(plane p, vec3 a, vec3 b, vec3 &intersect);
bool PlaneSphereIntersect(plane p, sphere s);
bool PlaneAABBIntersect(plane p, aabb b);

bool FrustumSegmentIntersect(frustum f, line l, vec3 &intersect);
bool FrustumSegmentIntersect(frustum f, vec3 a, vec3 b, vec3 &intersect);
bool FrustumSphereIntersect(frustum f, sphere s);
bool FrustumAABBIntersect(frustum f, aabb box);

#endif