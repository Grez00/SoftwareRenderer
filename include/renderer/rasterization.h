#ifndef RASTERIZATION_H
#define RASTERIZATION_H

#include "mat4.h"
#include "vectors.h"
#include "triangles.h"
#include "renderinfo.h"
#include "shader.h"
#include "geometry.h"

#include <math.h>

float Edge(vec3 v0, vec3 v1, vec2 p);
bool IsInTriangle(vec2 p, Triangle2D tri);
vec4 BarycentricCoords(vec2 p, Triangle2D tri);
void RasterizeTriangle(Triangle2D tri, FrameBuffer buffer, Texture tex, Shader shader);
void RasterizeTriangleOld(Triangle2D tri, FrameBuffer buffer, Texture tex, Shader shader);
void RasterizeLine(vec3 a, vec3 b, FrameBuffer buffer, vec3 col = vec3(1, 0, 0));

#endif