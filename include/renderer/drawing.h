#ifndef DRAWING_H
#define DRAWING_H

#include "mat3.h"
#include "mat4.h"
#include "vertex.h"
#include "rasterization.h"
#include "rendermats.h"
#include "renderinfo.h"
#include "framebuffer.h"
#include "mesh.h"
#include "shader.h"
#include "geometry.h"

vec3 ProjectVector(vec4 v, mat4 proj, int w, int h);
vertex2D ProjectVertex(vertex v, mat4 proj, int w, int h);
void ProjectLine(vec4 a, vec4 b, vec3& a_proj, vec3& b_proj, mat4 proj, int w, int h);
Triangle2D ProjectTriangle(Triangle3D tri, mat4 proj, int w, int h);

bool IsInFrustum(Mesh mesh, Camera cam, mat4 model);

int ClipTriangle(Triangle3D *tri, Triangle3D *tri2, aabb box);
int ClipTriangle(Triangle3D *tri, Triangle3D *tri2);
bool ClipLine(vec4 &a, vec4 &b);

void DrawAABB(aabb a, FrameBuffer buffer, mat4 proj);
void DrawLine(vec4 a, vec4 b, FrameBuffer buffer, mat4 proj, vec3 col = vec3(1, 0, 0));
void DrawLine(line line, FrameBuffer buffer, mat4 proj, vec3 col = vec3(1, 0, 0));
void DrawQuad(vec4 a, vec4 b, vec4 c, vec4 d, FrameBuffer buffer, mat4 proj, vec3 col);
void DrawFrustum(Camera cam, FrameBuffer buffer, mat4 proj, vec3 col);
void DrawPlane(plane p, FrameBuffer buffer, mat4 proj, vec3 col);
void DrawTriangle(Triangle3D tri, FrameBuffer buffer, mat4 proj, Texture tex, Shader shader);
void DrawTriangleWireframe(Triangle3D tri, FrameBuffer buffer, mat4 proj);
void DrawTriangles(vertex vertices[], int vert_count, RenderInfo render_info);
void DrawTrianglesIndexed(vertex vertices[], int indices[], int vert_count, int index_count, RenderInfo *render_info);
void DrawTriangleStrips(vertex vertices[], int vert_count, RenderInfo render_info);
void DrawMesh(Mesh mesh, FrameBuffer buffer, mat4 proj, mat4 model, Shader shader);

#endif