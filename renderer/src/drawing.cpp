#include "renderer/drawing.h"

// Projection

vec3 ProjectVector(vec4 v, mat4 proj, int w, int h){
    mat4 vp = ViewportMatrix(w, h);
    vec4 p_vec = vp * proj * v;
    return vec3(p_vec.x / p_vec.w, p_vec.y / p_vec.w, v.z);
}

vertex2D ProjectVertex(vertex v, mat4 proj, int w, int h){
    mat4 vp = ViewportMatrix(w, h);
    vec4 pos = proj * v.position;
    float depth = pos.w;
    pos/=pos.w;
    pos = vp * pos;
    return vertex2D(vec3(pos.x, pos.y, depth), v.normal, v.uv);
}

vertex2D WindowVertex(vertex v, int w, int h){
    mat4 vp = ViewportMatrix(w, h);
    vec4 pos = v.position;
    pos/=pos.w;
    pos = vp * pos;
    return vertex2D(vec3(pos.x, pos.y, v.position.w), v.normal, v.uv);
}

void ProjectLine(vec4 a, vec4 b, vec3& a_proj, vec3& b_proj, mat4 proj, int w, int h){
    a_proj = ProjectVector(a, proj, w, h);
    b_proj = ProjectVector(b, proj, w, h);
}

Triangle2D ProjectTriangle(Triangle3D tri, mat4 proj, int w, int h){
    return Triangle2D(
        ProjectVertex(tri.vertices[0], proj, w, h), 
        ProjectVertex(tri.vertices[1], proj, w, h), 
        ProjectVertex(tri.vertices[2], proj, w, h)
    );
}

Triangle2D WindowTriangle(Triangle3D tri, int w, int h){
    return Triangle2D(
        WindowVertex(tri.vertices[0], w, h), 
        WindowVertex(tri.vertices[1], w, h), 
        WindowVertex(tri.vertices[2], w, h)
    ); 
}

Triangle3D ApplyMatrixToTriangle(Triangle3D tri, mat4 m){
    return Triangle3D(
        vertex(m * tri.vertices[0].position, tri.vertices[0].normal, tri.vertices[0].uv),
        vertex(m * tri.vertices[1].position, tri.vertices[1].normal, tri.vertices[1].uv),
        vertex(m * tri.vertices[2].position, tri.vertices[2].normal, tri.vertices[2].uv)
    );
}

// Clipping

bool IsInView(vertex v){
    vec4 p = v.position;

    if (p.w <= 0.0f) return false;
    if (
        (p.x < -p.w || p.x > p.w) ||
        (p.y < -p.w || p.y > p.w) ||
        (p.z < -p.w || p.z > p.w)
    ) return false;

    return true;
}

bool IsInView(Triangle3D tri){
    int num_out = 0;
    for (int i = 0; i < 3; i++){
        if (!IsInView(tri.vertices[i])) num_out++;
    }
    if (num_out == 3) return false;
    return true;
}

int ClipTriangle(Triangle3D *tri, Triangle3D *tri2, aabb box){
    int num_in = 0;
    vertex *in = new vertex[3];

    int num_out = 0;
    vertex *out = new vertex[3];

    for (int i = 0; i < 3; i++){
        if (box.isinvolume(tri->vertices[i].position)){
            in[num_in] = tri->vertices[i];
            num_in++;
        }
        else{
            out[num_out] = tri->vertices[i];
            num_out++;
        }
    }

    if (num_out == 0){
        return 0;
    }
    else if (num_out == 3){
        return -1;
    }
    else if (num_out == 2){
        vec3 v0;
        if (!box.segmentintersect(out[0].position, in[0].position, v0)) return -1;

        vec3 v1;
        if (!box.segmentintersect(out[1].position, in[0].position, v1)) return -1;

        tri->vertices[0] = in[0];
        tri->vertices[1].position = vec4(v0.x, v0.y, v0.z, 1.0f);
        tri->vertices[2].position = vec4(v1.x, v1.y, v1.z, 1.0f);

        return 0;
    }
    else{
        vec3 v0;
        if (!box.segmentintersect(out[0].position, in[0].position, v0)) return -1;

        vec3 v1;
        if (!box.segmentintersect(out[0].position, in[1].position, v1)) return -1;

        tri->vertices[0] = in[0];
        tri->vertices[1] = in[1];
        tri->vertices[2] = vertex(vec4(v0.x, v0.y, v0.z, 1.0f), out[0].normal, out[0].uv);

        tri2->vertices[0] = vertex(vec4(v0.x, v0.y, v0.z, 1.0f), out[0].normal, out[0].uv);
        tri2->vertices[1] = in[1];
        tri2->vertices[2] = vertex(vec4(v1.x, v1.y, v1.z, 1.0f), out[0].normal, out[0].uv);

        return 1;
    }
}

int ClipTriangle(Triangle3D *tri, Triangle3D *tri2){
    float w = tri->vertices[0].position.w;
    return ClipTriangle(tri, tri2, aabb(vec3(), vec3(w, w, w)));
}

void ClipMesh(Mesh mesh){

}

// Drawing

void DrawLine(vec4 a, vec4 b, FrameBuffer buffer, mat4 proj, vec3 col){
    vec3 a_proj;
    vec3 b_proj;
    ProjectLine(a, b, a_proj, b_proj, proj, buffer.w, buffer.h);
    RasterizeLine(a_proj, b_proj, buffer, col);
}

void DrawLine(line line, FrameBuffer buffer, mat4 proj, vec3 col){
    DrawLine(v3tov4(line.a, 1.0f), v3tov4(line.b, 1.0f), buffer, proj, col);
}

void DrawAABB(aabb a, FrameBuffer buffer, mat4 proj){
    vec3 f_a = a.center + vec3(-a.extents.x, a.extents.y, -a.extents.z);
    vec3 f_b = a.center + vec3(a.extents.x, a.extents.y, -a.extents.z);
    vec3 f_c = a.center + vec3(-a.extents.x, -a.extents.y, -a.extents.z);
    vec3 f_d = a.center + vec3(a.extents.x, -a.extents.y, -a.extents.z);

    vec3 b_a = a.center + vec3(-a.extents.x, a.extents.y, a.extents.z);
    vec3 b_b = a.center + vec3(a.extents.x, a.extents.y, a.extents.z);
    vec3 b_c = a.center + vec3(-a.extents.x, -a.extents.y, a.extents.z);
    vec3 b_d = a.center + vec3(a.extents.x, -a.extents.y, a.extents.z);

    DrawLine(vec4(f_a.x, f_a.y, f_a.z, 1.0f), vec4(f_b.x, f_b.y, f_b.z, 1.0f), buffer, proj);
    DrawLine(vec4(f_a.x, f_a.y, f_a.z, 1.0f), vec4(f_c.x, f_c.y, f_c.z, 1.0f), buffer, proj);
    DrawLine(vec4(f_c.x, f_c.y, f_c.z, 1.0f), vec4(f_d.x, f_d.y, f_d.z, 1.0f), buffer, proj);
    DrawLine(vec4(f_d.x, f_d.y, f_d.z, 1.0f), vec4(f_b.x, f_b.y, f_b.z, 1.0f), buffer, proj);

    DrawLine(vec4(b_a.x, b_a.y, b_a.z, 1.0f), vec4(b_b.x, b_b.y, b_b.z, 1.0f), buffer, proj);
    DrawLine(vec4(b_a.x, b_a.y, b_a.z, 1.0f), vec4(b_c.x, b_c.y, b_c.z, 1.0f), buffer, proj);
    DrawLine(vec4(b_c.x, b_c.y, b_c.z, 1.0f), vec4(b_d.x, b_d.y, b_d.z, 1.0f), buffer, proj);
    DrawLine(vec4(b_d.x, b_d.y, b_d.z, 1.0f), vec4(b_b.x, b_b.y, b_b.z, 1.0f), buffer, proj);

    DrawLine(vec4(f_a.x, f_a.y, f_a.z, 1.0f), vec4(b_a.x, b_a.y, b_a.z, 1.0f), buffer, proj);
    DrawLine(vec4(f_b.x, f_b.y, f_b.z, 1.0f), vec4(b_b.x, b_b.y, b_b.z, 1.0f), buffer, proj);
    DrawLine(vec4(f_c.x, f_c.y, f_c.z, 1.0f), vec4(b_c.x, b_c.y, b_c.z, 1.0f), buffer, proj);
    DrawLine(vec4(f_d.x, f_d.y, f_d.z, 1.0f), vec4(b_d.x, b_d.y, b_d.z, 1.0f), buffer, proj);
}

void DrawPlane(plane p, FrameBuffer buffer, mat4 proj, vec3 col){
    vec3 normal_point = p.d + p.n;
    DrawLine(vec4(p.d.x, p.d.y, p.d.z, 1.0f), vec4(normal_point.x, normal_point.y, normal_point.z, 1.0f), buffer, proj, col);
}

void DrawTriangle(Triangle3D tri, FrameBuffer buffer, mat4 proj, Texture tex, Shader shader){
    Triangle3D main_tri = ApplyMatrixToTriangle(tri, proj);
    if (!IsInView(main_tri)) return;
    RasterizeTriangle(WindowTriangle(main_tri, buffer.w, buffer.h), buffer, tex, shader);
}

void DrawTriangleWireframe(Triangle3D tri, FrameBuffer buffer, mat4 proj){
    DrawLine(tri.vertices[0].position, tri.vertices[1].position, buffer, proj);
    DrawLine(tri.vertices[1].position, tri.vertices[2].position, buffer, proj);
    DrawLine(tri.vertices[2].position, tri.vertices[0].position, buffer, proj);
}

/*
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