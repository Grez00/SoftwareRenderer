#include "renderer/rendermats.h"

// Change of Coordinates

// COC with three axes and origin
mat4 GetCOCMatrix(vec3 u, vec3 v, vec3 w, vec3 p){
    return mat4(
        u.x, v.x, w.x, p.x,
        u.y, v.y, w.y, p.y,
        u.z, v.z, w.z, p.z,
        0, 0, 0, 1
    );
}

// COC with two axes and origin
mat4 GetCOCMatrix(vec3 a, vec3 b, vec3 p){
    vec3 u = normalize(a);
    vec3 w = normalize(cross(u, b));
    vec3 v = cross(w, u);
    return GetCOCMatrix(u, v, w, p);
}

// COC with one axis and origin
mat4 GetCOCMatrix(vec3 a, vec3 p){
    vec3 b = vec3(1, 0, 0);
    if (dot(a, b) > dot(a, vec3(0, 1, 0))){
        b = vec3(0, 1, 0);
    }
    if (dot(a, b) > dot(a, vec3(0, 0, 1))){
        b = vec3(0, 0, 1);
    }

    vec3 u = normalize(a);
    vec3 w = normalize(cross(u, b));
    vec3 v = cross(w, u);
    return GetCOCMatrix(u, v, w, p);
}

// Viewing

// TODO
mat4 LookAt(vec3 position, vec3 target, vec3 up){
    return mat4::Identity();
}

// Windowing

mat4 Get2DWindowingMatrix(vec2 sl, vec2 sh, vec2 el, vec2 eh){
    return mat4(
        (eh.x - el.x) / (sh.x - sl.x), 0, 0, ((el.x * sh.x) - (eh.x * sl.x)) / (sh.x - sl.x),
        0, (eh.y - el.y) / (sh.y - sl.y), 0, ((el.y * sh.y) - (eh.y * sl.y)) / (sh.y - sl.y),
        0, 0, 1, 0,
        0, 0, 0, 1
    );
}

mat4 Get3DWindowingMatrix(vec3 sl, vec3 sh, vec3 el, vec3 eh){
    return mat4(
        (eh.x - el.x) / (sh.x - sl.x), 0, 0, ((el.x * sh.x) - (eh.x * sl.x)) / (sh.x - sl.x),
        0, (eh.y - el.y) / (sh.y - sl.y), 0, ((el.y * sh.y) - (eh.y * sl.y)) / (sh.y - sl.y),
        0, 0, (eh.z = el.z) / (sh.z - sl.z), ((el.z * sh.z) - (eh.z * sl.z)) / (sh.z - sl.z),
        0, 0, 0, 1
    );
}

mat4 ViewportMatrix(int w, int h){
    return Get2DWindowingMatrix(vec2(-1, -1), vec2(1, 1), vec2(0, 0), vec2(w - 1, h - 1));
}

// Projection

mat4 Perspective(float n, float f){
    return mat4(
        n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, n+f, n*f,
        0, 0, -1, 0
    );
}

// TODO
mat4 Orthographic(float l, float r, float b, float t, float n, float f){
    return Get3DWindowingMatrix(vec3(l, b, n), vec3(r, t, f), vec3(-1, -1, -1), vec3(1, 1, 1));
}

// TODO
mat4 GetProjectionMatrix(float fov, float aspect_ratio, float n, float f){
    float h = tan(M_PI / 180.0f * fov/2.0f) * 2.0f * n;
    float w = aspect_ratio * h;

    float l = -w/2.0f;
    float r = w/2.0f;
    float b = -h/2.0f;
    float t = h/2.0f;
    return Orthographic(l, r, b, t, n, f) * Perspective(n, f);
}