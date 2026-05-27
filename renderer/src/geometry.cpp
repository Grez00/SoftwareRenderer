#include "renderer/geometry.h"

// AABB

aabb::aabb(vec3 p_center, vec3 p_extents){
    center = p_center;
    extents = p_extents;
}

bool aabb::isinside(vec3 q){
    vec3 v = q - center;
    return (
        v.x >= -extents.x && v.x <= extents.x && 
        v.y >= -extents.y && v.y <= extents.y &&
        v.z >= -extents.z && v.z <= extents.z
    );
}

// PLANE

plane::plane(vec3 point, vec3 normal){
    p = point;
    n = normal;
    d = dot(n, p);
}

plane::plane(){
    p = vec3();
    n = vec3(0, 1, 0);
    d = dot(n, p);
}

bool plane::distance(vec3 q){
    return dot(n, q-d);
}

bool plane::isinside(vec3 q){
    return distance(q) < 0;
}

vec3 plane::projectpoint(vec3 q){
    return q - distance(q)*n;
}

// LINE

line::line(vec3 p_a, vec3 p_b){
    a = p_a;
    b = p_b;
}

vec3 line::getpoint(float t){
    return a + t*(b - a);
}

// SPHERE

sphere::sphere(vec3 p_c, float p_r){
    c = p_c;
    r = p_r;
}

// FRUSTUM

frustum::frustum(plane *p_planes){
    for (int i = 0; i < 6; i++){
        planes[i] = p_planes[i];
    }
}

frustum::frustum(Camera cam){
    float near_height = tan(cam.fov / 2.0f) * cam.near * 2.0f;
    float near_width = near_height * cam.aspect;
    vec3 near_center = cam.position + cam.forward * cam.near;
    vec3 near_normal = cam.forward;

    vec3 far_center = cam.position + cam.forward * cam.far;
    vec3 far_normal = -cam.forward;

    vec3 left_normal = normalize(cross(cam.up, (near_center - (cam.right * near_width / 2.0f)) - cam.position));
    vec3 right_normal = normalize(cross(cam.up, (near_center + (cam.right * near_width / 2.0f)) - cam.position));
    vec3 top_normal = normalize(cross(cam.right, (near_center + (cam.up * near_height / 2.0f)) - cam.position));
    vec3 bottom_normal = normalize(cross(cam.right, (near_center - (cam.up * near_height / 2.0f)) - cam.position));

    planes[0] = plane(cam.position, bottom_normal);
    planes[1] = plane(cam.position, top_normal);
    planes[2] = plane(cam.position, left_normal);
    planes[3] = plane(cam.position, right_normal);
    planes[4] = plane(near_center, near_normal);
    planes[5] = plane(far_center, far_normal);
}

// INTERSECTS

bool TestAxis(float d, float p, float min, float max, float &tmin, float &tmax){
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

bool AABBSegmentIntersect(aabb box, vec3 a, vec3 b, vec3 &intersect){
    vec3 dir = normalize(b - a);

    intersect = vec3();

    float tmin = 0.0f;
    float tmax = dot(b - a, dir);

    vec3 p = a - box.center;
    vec3 d = dir;

    vec3 min = -box.extents;
    vec3 max = box.extents;

    if (!(TestAxis(d.x, p.x, min.x, max.x, tmin, tmax) 
    && TestAxis(d.y, p.y, min.y, max.y, tmin, tmax)
    && TestAxis(d.z, p.z, min.z, max.z, tmin, tmax))) return false;

    intersect = (p + d * tmin) + box.center;

    return true;
}
bool AABBSegmentIntersect(aabb box, line l, vec3 &intersect){
    return AABBSegmentIntersect(box, l.a, l.b, intersect);
}

bool PlaneSegmentIntersect(plane p, vec3 a, vec3 b, vec3 &intersect){
    vec3 dir = normalize(b-a);
    float dot_normal_dir = dot(p.n, dir);
    if (dot_normal_dir < 0.0001f) return false;

    float t = (p.d - dot(p.n, a)) / dot_normal_dir;

    if (t < 0.0f || t > dot(b-a, dir)) return false;

    intersect = a + t*dir;
    return true;
}
bool PlaneSegmentIntersect(plane p, line l, vec3 &intersect){
    return PlaneSegmentIntersect(p, l.a, l.b, intersect);
}

bool PlaneSphereIntersect(plane p, sphere s){
    if (abs(p.distance(s.c)) > s.r) return false;

    return true;
}

bool PlaneAABBIntersect(plane p, aabb b){
    float r = dot(b.extents, abs(p.n));
    float s = p.distance(b.center);

    return abs(s) <= r;
}

bool FrustumSegmentIntersect(frustum f, line l, vec3 &intersect){
    return FrustumSegmentIntersect(f, l.a, l.b, intersect);
}
bool FrustumSegmentIntersect(frustum f, vec3 a, vec3 b, vec3 &intersect){
    bool hit = false;
    intersect = b;
    vec3 temp_intersect = a;
    for (plane p : f.planes){
        if (PlaneSegmentIntersect(p, a, b, temp_intersect)){
            hit = true;
            if (distance(temp_intersect, a) < distance(intersect, a)){
                intersect = temp_intersect;
            }
        }
    }

    return hit;
}

bool FrustumSphereIntersect(frustum f, sphere s){
    bool hit = false;
    for (plane p : f.planes){
        if (PlaneSphereIntersect(p, s)) return true;
    }

    return hit;
}

bool FrustumAABBIntersect(frustum f, aabb box){
    bool hit = false;
    for (plane p : f.planes){
        if (PlaneAABBIntersect(p, box)) return true;
    }

    return false;
}