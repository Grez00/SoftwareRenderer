#include "renderer/geometry.h"

// AABB

aabb::aabb(vec3 p_center, vec3 p_extents){
    center = p_center;
    extents = p_extents;
}

bool aabb::isinvolume(vec3 p){
    vec3 v = p - center;
    return (
        v.x >= -extents.x && v.x <= extents.x && 
        v.y >= -extents.y && v.y <= extents.y &&
        v.z >= -extents.z && v.z <= extents.z
    );
}

bool testaxis(float d, float p, float min, float max, float &tmin, float &tmax){
    if (abs(d) < 0.000001f)
    {
        if (p < min || p > max) return false;
    }
    else
    {
        float t1 = (min - p) / d;
        float t2 = (max - p) / d;

        if (t1 > t2) std::swap(t1, t2);

        if (t1 > tmin) tmin = t1;
        if (t2 < tmax) tmax = t2;

        if (tmin > tmax) return false;
    }
    return true;
}

bool aabb::segmentintersect(vec3 a, vec3 b, vec3 &intersect){
    vec3 dir = normalize(b - a);

    intersect = vec3();

    float tmin = 0.0f;
    float tmax = dot(b - a, dir);

    vec3 p = a - center;
    vec3 d = dir;

    vec3 min = -extents;
    vec3 max = extents;

    if (!(testaxis(d.x, p.x, min.x, max.x, tmin, tmax) 
    && testaxis(d.y, p.y, min.y, max.y, tmin, tmax) 
    && testaxis(d.z, p.z, min.z, max.z, tmin, tmax))) return false;

    intersect = (p + d * tmin) + center;

    return true;
}

// PLANE

plane::plane(vec3 point, vec3 normal){
    d = point;
    n = normal;
}

bool plane::isinside(vec3 q){
    return false; // TODO
}

bool plane::segmentintersect(vec3 a, vec3 b, vec3 &intersect){
    return false; // TODO
}

// LINE

line::line(vec3 p_a, vec3 p_b){
    a = p_a;
    b = p_b;
}

vec3 line::getpoint(float t){
    return a + t*(b - a);
}