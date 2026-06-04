#include "renderer/drawing.h"

// Projection

vec3 ProjectVector(vec4 v, mat4 proj, int w, int h){
    mat4 vp = ViewportMatrix(w, h);
    vec4 p_vec = vp * proj * v;
    return vec3(p_vec.x / p_vec.w, p_vec.y / p_vec.w, v.z);
}

vec3 WindowVector(vec4 v, mat4 proj, int w, int h){
    mat4 vp = ViewportMatrix(w, h);
    vec4 pos = v;
    pos/=pos.w;
    pos = vp * pos;
    return vec3(int(pos.x), int(pos.y), v.w);
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

void ProjectLine(vec4 a, vec4 b, vec3 &a_proj, vec3 &b_proj, mat4 proj, int w, int h){
    a_proj = ProjectVector(a, proj, w, h);
    b_proj = ProjectVector(b, proj, w, h);
}

void WindowLine(vec4 a, vec4 b, vec3 &a_window, vec3 &b_window, mat4 proj, int w, int h){
    a_window = WindowVector(a, proj, w, h);
    b_window = WindowVector(b, proj, w, h);
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

bool IsInView(vec4 p){
    if (p.w <= 0.0f) return false;
    if (
        (p.x < -p.w || p.x > p.w) ||
        (p.y < -p.w || p.y > p.w) ||
        (p.z < -p.w || p.z > p.w)
    ) return false;

    return true;   
}

bool IsInView(vertex v){
    return IsInView(v.position);
}

bool IsInView(Triangle3D tri){
    int num_out = 0;
    for (int i = 0; i < 3; i++){
        if (!IsInView(tri.vertices[i])) num_out++;
    }
    if (num_out == 3) return false;
    return true;
}

bool IsBackface(Triangle3D tri, FrameBuffer buffer, mat4 proj){
    vec3 v0 = vec3(tri.vertices[1].position - tri.vertices[0].position);
    vec3 v1 = vec3(tri.vertices[2].position - tri.vertices[0].position);
    vec3 normal = normalize(cross(v0, v1));
    vec3 view = tri.vertices[0].position;

    if (dot(normal, view) >= 0.0f) return true;

    return false;
}

bool IsInFrustum(Mesh mesh, Camera cam, mat4 model){
    sphere bounding_sphere = mesh.GetBoundingSphere();
    bounding_sphere.c = model * v3tov4(bounding_sphere.c, 1.0f);
    frustum cam_frustum = frustum(cam);
    return FrustumSphereIntersect(cam_frustum, bounding_sphere);
}

bool LineAxisIntersect(float a_v, float b_v, vertex &a, vertex &b, vertex &intersect){
    bool a_in = a.position.w >= a_v;
    bool b_in = b.position.w >= b_v;

    if (!(a_in || b_in)) return -1;
    if (!(a_in && b_in)){
        float t = (a.position.w - a_v) / ((a.position.w - a_v) - (b.position.w - b_v));
        intersect.position = a.position*(1.0f-t) + b.position*t;
        intersect.normal = a.normal*(1.0f-t) + b.normal*t;
        intersect.uv = a.uv*(1.0f-t) + b.uv*t;

        return 0;
    }

    return -1;
}

ClipLineResult ClipLineAxis(float a_v, float b_v, vec4 &a, vec4 &b){
    bool a_in = a.w >= a_v;
    bool b_in = b.w >= b_v;

    if (!(a_in || b_in)) return LINE_ALL_OUT;
    if (!(a_in && b_in)){
        float t = (a.w - a_v) / ((a.w - a_v) - (b.w - b_v));
        vec4 intersection = a*(1.0f-t) + b*t;

        if (a_in){
            b = intersection;
            return LINE_B_OUT;
        }
        else{
            a = intersection;
            return LINE_A_OUT;
        }
    }

    return LINE_ALL_IN;
}

bool ClipLine(vec4 &a, vec4 &b){
    float epsilon = 0.00001f;

    if (ClipLineAxis(epsilon, epsilon, a, b) == LINE_ALL_OUT) return false;
    if (ClipLineAxis(a.y, b.y, a, b) == LINE_ALL_OUT) return false;
    if (ClipLineAxis(-a.y, -b.y, a, b) == LINE_ALL_OUT) return false;
    if (ClipLineAxis(a.x, b.x, a, b) == LINE_ALL_OUT) return false;
    if (ClipLineAxis(-a.x, -b.x, a, b) == LINE_ALL_OUT) return false;
    if (ClipLineAxis(a.z, b.z, a, b) == LINE_ALL_OUT) return false;
    if (ClipLineAxis(-a.z, -b.z, a, b) == LINE_ALL_OUT) return false;

    return true; // Passed all tests
}

std::vector<Triangle3D> ClipTriangleAxis(float t1, float t2, float t3, Triangle3D *tri){
    std::vector<Triangle3D> tris = std::vector<Triangle3D>();
    float *test_values = new float[3];
    test_values[0] = t1;
    test_values[1] = t2;
    test_values[2] = t3;

    int num_in = 0;
    int *in = new int[3];

    int num_out = 0;
    int *out = new int[3];

    for (int i = 0; i < 3; i++){
        if (tri->vertices[i].position.w >= test_values[i]){
            in[num_in] = i;
            num_in++;
        }
        else{
            out[num_out] = i;
            num_out++;
        }
    }

    if (num_out == 0){
        tris.push_back(*tri);
        return tris;
    }
    else if (num_out == 3){
        return tris;
    }
    else if (num_out == 2){
        vertex v0;
        if (LineAxisIntersect(test_values[in[0]], test_values[out[0]], tri->vertices[in[0]], tri->vertices[out[0]], v0) == -1) return tris;

        vertex v1;
        if (LineAxisIntersect(test_values[in[0]], test_values[out[1]], tri->vertices[in[0]], tri->vertices[out[1]], v1) == -1) return tris;

        tris.push_back(
            Triangle3D(
                tri->vertices[in[0]],
                v0,
                v1
            )
        );

        return tris;
    }
    else{
        vertex v0;
        if (LineAxisIntersect(test_values[in[0]], test_values[out[0]], tri->vertices[in[0]], tri->vertices[out[0]], v0) == -1) return tris;

        vertex v1;
        if (LineAxisIntersect(test_values[in[1]], test_values[out[0]], tri->vertices[in[1]], tri->vertices[out[0]], v1) == -1) return tris;

        tris.push_back(
            Triangle3D(
                tri->vertices[in[0]],
                tri->vertices[in[1]],
                v0
            )
        );
        tris.push_back(
            Triangle3D(
                v0,
                tri->vertices[in[1]],
                v1
            )
        );

        return tris;
    }
}

std::vector<Triangle3D> ClipTriangles(std::vector<Triangle3D> *tris){
    float epsilon = 0.00001f;

    std::vector<Triangle3D> result_0 = std::vector<Triangle3D>();
    result_0.insert(result_0.end(), tris->begin(), tris->end());

    std::vector<Triangle3D> result_1 = std::vector<Triangle3D>();
    std::vector<Triangle3D> next_result = std::vector<Triangle3D>();

    for (Triangle3D tri : result_0){
        next_result = ClipTriangleAxis(epsilon, epsilon, epsilon, &tri);
        result_1.insert(result_1.end(), next_result.begin(), next_result.end());
    }
    result_0.clear();

    for (Triangle3D tri : result_1){
        next_result = ClipTriangleAxis(tri.vertices[0].position.x, tri.vertices[1].position.x, tri.vertices[2].position.x, &tri);
        result_0.insert(result_0.end(), next_result.begin(), next_result.end());
    }
    result_1.clear();

    for (Triangle3D tri : result_0){
        next_result = ClipTriangleAxis(-tri.vertices[0].position.x, -tri.vertices[1].position.x, -tri.vertices[2].position.x, &tri);
        result_1.insert(result_1.end(), next_result.begin(), next_result.end());
    }
    result_0.clear();

    for (Triangle3D tri : result_1){
        next_result = ClipTriangleAxis(tri.vertices[0].position.y, tri.vertices[1].position.y, tri.vertices[2].position.y, &tri);
        result_0.insert(result_0.end(), next_result.begin(), next_result.end());
    }
    result_1.clear();

    for (Triangle3D tri : result_0){
        next_result = ClipTriangleAxis(-tri.vertices[0].position.y, -tri.vertices[1].position.y, -tri.vertices[2].position.y, &tri);
        result_1.insert(result_1.end(), next_result.begin(), next_result.end());
    }
    result_0.clear();

    for (Triangle3D tri : result_1){
        next_result = ClipTriangleAxis(tri.vertices[0].position.z, tri.vertices[1].position.z, tri.vertices[2].position.z, &tri);
        result_0.insert(result_0.end(), next_result.begin(), next_result.end());
    }
    result_1.clear();

    for (Triangle3D tri : result_0){
        next_result = ClipTriangleAxis(-tri.vertices[0].position.z, -tri.vertices[1].position.z, -tri.vertices[2].position.z, &tri);
        result_1.insert(result_1.end(), next_result.begin(), next_result.end());
    }
    result_0.clear();

    return result_1; // TODO
}

std::vector<Triangle3D> ClipTriangle(Triangle3D *tri){
    std::vector<Triangle3D> tris = std::vector<Triangle3D>();
    tris.push_back(*tri);

    return ClipTriangles(&tris);
}

// Drawing

void DrawPoint(vec4 v, FrameBuffer buffer, mat4 proj, vec3 col){
    vec3 v_window = ProjectVector(v, proj, buffer.w, buffer.h);
    if (buffer.IsOOB(vec2(v_window.x, v_window.y))) return;

    buffer.SetRenderBuffer(v_window.x, v_window.y, col);
}

void DrawLine(vec4 a, vec4 b, FrameBuffer buffer, mat4 proj, vec3 col){
    vec4 a_proj = proj * a;
    vec4 b_proj = proj * b;

    if (!ClipLine(a_proj, b_proj)) return;

    vec3 a_window;
    vec3 b_window;
    WindowLine(a_proj, b_proj, a_window, b_window, proj, buffer.w, buffer.h);
    RasterizeLine(a_window, b_window, buffer, col);
}

void DrawLine(line line, FrameBuffer buffer, mat4 proj, vec3 col){
    DrawLine(v3tov4(line.a, 1.0f), v3tov4(line.b, 1.0f), buffer, proj, col);
}

void DrawQuad(vec4 a, vec4 b, vec4 c, vec4 d, FrameBuffer buffer, mat4 proj, vec3 col){
    DrawLine(a, b, buffer, proj, col);
    DrawLine(a, c, buffer, proj, col);
    DrawLine(d, b, buffer, proj, col);
    DrawLine(d, c, buffer, proj, col);
}

void DrawAxes(Camera cam, FrameBuffer buffer, mat4 proj){
    DrawLine(v3tov4(cam.position, 1.0f), v3tov4(cam.position + cam.right, 1.0f), buffer, proj, vec3(1, 0, 0));
    DrawLine(v3tov4(cam.position, 1.0f), v3tov4(cam.position + cam.up, 1.0f), buffer, proj, vec3(0, 1, 0));
    DrawLine(v3tov4(cam.position, 1.0f), v3tov4(cam.position + cam.forward, 1.0f), buffer, proj, vec3(0, 0, 1));
}

void DrawFrustum(Camera cam, FrameBuffer buffer, mat4 proj, vec3 col){
    float partial = tan(M_PI/180.0f * cam.fov/2.0f);

    float near_extent_h = partial * cam.near;
    float near_extent_w = near_extent_h * cam.aspect;
    vec3 near_center = cam.position + cam.forward * cam.near;

    vec4 n_a = v3tov4(near_center + cam.up * near_extent_h - cam.right * near_extent_w, 1.0f);
    vec4 n_b = v3tov4(near_center + cam.up * near_extent_h + cam.right * near_extent_w, 1.0f);
    vec4 n_c = v3tov4(near_center - cam.up * near_extent_h - cam.right * near_extent_w, 1.0f);
    vec4 n_d = v3tov4(near_center - cam.up * near_extent_h + cam.right * near_extent_w, 1.0f);

    float far_extent_h = partial * cam.far;
    float far_extent_w = far_extent_h * cam.aspect;
    vec3 far_center = cam.position + cam.forward * cam.far;

    vec4 f_a = v3tov4(far_center + cam.up * far_extent_h - cam.right * far_extent_w, 1.0f);
    vec4 f_b = v3tov4(far_center + cam.up * far_extent_h + cam.right * far_extent_w, 1.0f);
    vec4 f_c = v3tov4(far_center - cam.up * far_extent_h - cam.right * far_extent_w, 1.0f);
    vec4 f_d = v3tov4(far_center - cam.up * far_extent_h + cam.right * far_extent_w, 1.0f);

    DrawQuad(n_a, n_b, n_c, n_d, buffer, proj, col);
    DrawQuad(f_a, f_b, f_c, f_d, buffer, proj, col);
    DrawQuad(n_a, f_a, n_c, f_c, buffer, proj, col);
    DrawQuad(n_b, f_b, n_d, f_d, buffer, proj, col);
    DrawQuad(n_a, f_a, n_b, f_b, buffer, proj, col);
    DrawQuad(n_c, f_c, n_d, f_d, buffer, proj, col);
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
    vec3 normal_point = p.p + p.n;
    DrawLine(v3tov4(p.p, 1.0f), v3tov4(normal_point, 1.0f), buffer, proj, col);
    DrawPoint(v3tov4(p.p, 1.0f), buffer, proj, vec3(1.0f - col.x, 1.0f - col.y, 1.0f - col.z));
}

void DrawSphere(sphere s, FrameBuffer buffer, mat4 proj, vec3 col){
    DrawLine(v3tov4(s.c + vec3(-1, 0, 0) * s.r, 1.0f), v3tov4(s.c + vec3(1, 0, 0) * s.r, 1.0f), buffer, proj, vec3(1, 0, 0));
    DrawLine(v3tov4(s.c + vec3(0, -1, 0) * s.r, 1.0f), v3tov4(s.c + vec3(0, 1, 0) * s.r, 1.0f), buffer, proj, vec3(0, 1, 0));
    DrawLine(v3tov4(s.c + vec3(0, 0, -1) * s.r, 1.0f), v3tov4(s.c + vec3(0, 0, 1) * s.r, 1.0f), buffer, proj, vec3(0, 0, 1));
}

void DrawTriangle(Triangle3D tri, FrameBuffer buffer, mat4 proj, Texture tex, Shader shader){
    if (IsBackface(tri, buffer, proj)) return;
    Triangle3D main_tri = ApplyMatrixToTriangle(tri, proj);
    std::vector<Triangle3D> clipped_tris = ClipTriangle(&main_tri);

    for (Triangle3D clipped_tri : clipped_tris){
        RasterizeTriangle(WindowTriangle(clipped_tri, buffer.w, buffer.h), buffer, tex, shader);
    }
}

void DrawTriangleWireframe(Triangle3D tri, FrameBuffer buffer, mat4 proj, vec3 col){
    DrawLine(tri.vertices[0].position, tri.vertices[1].position, buffer, proj, col);
    DrawLine(tri.vertices[1].position, tri.vertices[2].position, buffer, proj, col);
    DrawLine(tri.vertices[2].position, tri.vertices[0].position, buffer, proj, col);
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

void DrawMeshWireframe(Mesh mesh, FrameBuffer buffer, mat4 proj, mat4 model, vec3 col){
    mat3 id_model = mat3(transpose(inverse(model)));
    for (int i = 0; i < mesh.index_count; i+=3){
        DrawTriangleWireframe(
            Triangle3D(
                vertex(model * mesh.positions[int(mesh.indices[i].x)], id_model * mesh.normals[int(mesh.indices[i].z)], mesh.uvs[int(mesh.indices[i].y)]), 
                vertex(model * mesh.positions[int(mesh.indices[i+1].x)], id_model * mesh.normals[int(mesh.indices[i+1].z)], mesh.uvs[int(mesh.indices[i+1].y)]), 
                vertex(model * mesh.positions[int(mesh.indices[i+2].x)], id_model * mesh.normals[int(mesh.indices[i+2].z)], mesh.uvs[int(mesh.indices[i+2].y)])
            ), 
            buffer,
            proj,
            col
        );
    }
}