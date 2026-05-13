#include "renderer/drawing.h"

// Projection

vec3 ProjectVector(vec4 v, mat4 proj, int w, int h){
    mat4 vp = ViewportMatrix(w, h);
    vec4 p_vec = vp * proj * v;
    return vec3(p_vec.x / p_vec.w, p_vec.y / p_vec.w, v.z);
}

vertex2D ProjectVertex(vertex v, mat4 proj, int w, int h){
    mat4 vp = ViewportMatrix(w, h);
    vec4 p_vec = proj * v.position; // Apply projection (now in clip space, range -w to w)
    p_vec/=p_vec.w; // Perspective divide (now in NDC, range -1 to 1)
    p_vec = vp * p_vec; // viewport transform (now in screen coordinates)
    return vertex2D(vec3(p_vec.x, p_vec.y, v.position.z), v.normal, v.uv);
}

void ProjectLine(vec4 a, vec4 b, vec3& a_proj, vec3& b_proj, mat4 proj, int w, int h){
    a_proj = ProjectVector(a, proj, w, h);
    b_proj = ProjectVector(b, proj, w, h);
}

Triangle2D ProjectTriangle(Triangle3D tri, mat4 transform, int w, int h){
    return Triangle2D(
        ProjectVertex(tri.vertices[0], transform, w, h), 
        ProjectVertex(tri.vertices[1], transform, w, h), 
        ProjectVertex(tri.vertices[2], transform, w, h)
    );
}

// Clipping

bool ClipTriangle(Triangle3D tri, mat4 proj){
    return false;
}

void ClipMesh(Mesh mesh){

}

// Drawing

void DrawLine(vec4 a, vec4 b, FrameBuffer buffer, mat4 proj){
    vec3 a_proj;
    vec3 b_proj;
    ProjectLine(a, b, a_proj, b_proj, proj, buffer.w, buffer.h);
    RasterizeLine(a_proj, b_proj, buffer);
}

void DrawTriangle(Triangle3D tri, FrameBuffer buffer, mat4 proj, Texture tex, Shader shader){
    RasterizeTriangle(ProjectTriangle(tri, proj, buffer.w, buffer.h), buffer, tex, shader);
}

/*
void DrawTriangleWireframe(Triangle3D tri, RenderInfo render_info){
    DrawLine(tri.vertices[0].position, tri.vertices[1].position, render_info.buffer, render_info.transform);
    DrawLine(tri.vertices[1].position, tri.vertices[2].position, render_info.buffer, render_info.transform);
    DrawLine(tri.vertices[2].position, tri.vertices[0].position, render_info.buffer, render_info.transform);
}

void DrawTriangles(vertex vertices[], int vert_count, RenderInfo render_info){
    if (vert_count % 3 != 0) return; // number of vertices must be a multiple of three
    for (int i = 0; i < vert_count; i+=3){
        DrawTriangle(Triangle3D(vertices[i], vertices[i+1], vertices[i+2]), render_info);
    }
}

void DrawTrianglesIndexed(vertex *vertices, int *indices, int vert_count, int index_count, RenderInfo render_info){
    if (index_count % 3 != 0) return; // number of indices should be a multiple of three
    for (int i = 0; i < index_count; i+=3){
        DrawTriangle(Triangle3D(vertices[indices[i]], vertices[indices[i+1]], vertices[indices[i+2]]), render_info);
    }
}

void DrawTriangleStrips(vertex vertices[], int vert_count, RenderInfo render_info){
    if (vert_count < 3) return; // vertex count should be at least three
    for (int i = 0; i < vert_count - 2; i++){
        if (i % 2 == 0) DrawTriangle(Triangle3D(vertices[i+1], vertices[i], vertices[i+2]), render_info);
        else DrawTriangle(Triangle3D(vertices[i], vertices[i+1], vertices[i+2]), render_info);
    } 
}
*/

void DrawMesh(Mesh mesh, FrameBuffer buffer, mat4 proj, mat4 model, Shader shader){
    mat3 id_model = mat3(transpose(inverse(model)));
    for (int i = 0; i < mesh.index_count; i+=3){
        DrawTriangle(
            Triangle3D(
                vertex(model * mesh.positions[int(mesh.indices[i].x)], id_model * mesh.normals[int(mesh.indices[i].z)], mesh.uvs[int(mesh.indices[i].y)]), 
                vertex(model * mesh.positions[int(mesh.indices[i+1].x)], id_model * mesh.normals[int(mesh.indices[i+1].z)], mesh.uvs[int(mesh.indices[i+1].y)]), 
                vertex(model * mesh.positions[int(mesh.indices[i+2].x)], id_model * mesh.normals[int(mesh.indices[i+2].z)], mesh.uvs[int(mesh.indices[i+2].y)])
            ), 
            buffer,
            proj,
            mesh.tex,
            shader
        );
    }
}