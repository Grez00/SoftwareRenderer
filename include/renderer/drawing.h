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

vec3 ProjectVector(vec4 v, mat4 proj, int w, int h);
vertex2D ProjectVertex(vertex v, mat4 proj, int w, int h);
void ProjectLine(vec4 a, vec4 b, vec3& a_proj, vec3& b_proj, mat4 proj, int w, int h);
Triangle2D ProjectTriangle(Triangle3D tri, mat4 proj, int w, int h);

void DrawLine(vec4 a, vec4 b, FrameBuffer buffer, mat4 proj);
void DrawTriangle(Triangle3D tri, FrameBuffer buffer, mat4 proj, Texture tex, Shader shader);
void DrawTriangleWireframe(Triangle3D tri, RenderInfo render_info);
void DrawTriangles(vertex vertices[], int vert_count, RenderInfo render_info);
void DrawTrianglesIndexed(vertex vertices[], int indices[], int vert_count, int index_count, RenderInfo *render_info);
void DrawTriangleStrips(vertex vertices[], int vert_count, RenderInfo render_info);
void DrawMesh(Mesh mesh, FrameBuffer buffer, mat4 proj, mat4 model, Shader shader);

#endif