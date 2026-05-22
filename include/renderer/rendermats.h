#ifndef RENDERMATS_H
#define RENDERMATS_H

#include "mat4.h"
#include "vectors.h"
#include <math.h>

mat4 GetCOCMatrix(vec3 u, vec3 v, vec3 w, vec3 p);
mat4 GetCOCMatrix(vec3 a, vec3 b, vec3 p);
mat4 GetCOCMatrix(vec3 a, vec3 p);

mat4 LookAt(vec3 position, vec3 target, vec3 up);

mat4 Get2DWindowingMatrix(vec2 sl, vec2 sh, vec2 el, vec2 eh);
mat4 Get3DWindowingMatrix(vec3 sl, vec3 sh, vec3 el, vec3 eh);
mat4 ViewportMatrix(int w, int h);

mat4 Perspective(float n, float f);
mat4 Orthographic(float l, float r, float b, float t, float n, float f);
mat4 GetProjectionMatrix(float fov, float aspect_ratio, float n, float f);

#endif