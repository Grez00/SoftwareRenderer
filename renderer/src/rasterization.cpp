#include "renderer/rasterization.h"

// Triangle Collision and Rasterization

float Edge(vec3 v0, vec3 v1, vec2 p){
    return (p.x - v0.x) * (v1.y - v0.y) - (p.y - v0.y) * (v1.x - v0.x);
}

bool IsInTriangle(vec2 p, Triangle2D tri){
    float e0 = Edge(tri.vertices[0].position, tri.vertices[1].position, p);
    float e1 = Edge(tri.vertices[1].position, tri.vertices[2].position, p);
    float e2 = Edge(tri.vertices[2].position, tri.vertices[0].position, p);

    if (e0 >= 0.0f && e1 >= 0.0f && e2 >= 0.0f){
        return true;
    }
    return false;
}

vec4 BarycentricCoords(vec2 p, Triangle2D tri){
    float e0 = Edge(tri.vertices[0].position, tri.vertices[1].position, p);
    float e1 = Edge(tri.vertices[1].position, tri.vertices[2].position, p);
    float e2 = Edge(tri.vertices[2].position, tri.vertices[0].position, p);

    float double_tri_area = Edge(
        tri.vertices[1].position, 
        tri.vertices[2].position, 
        vec2(tri.vertices[0].position.x, tri.vertices[0].position.y)
    );
    return vec4(
        e1/double_tri_area,
        e2/double_tri_area,
        e0/double_tri_area,
        (e0 > 0.0f && e1 > 0.0f && e2 > 0.0f) ? 1.0f : 0.0f
    );
}

vec4 GetBoundingBox(Triangle2D tri){
    vec3 po = tri.vertices[0].position;
    vec3 p1 = tri.vertices[1].position;
    vec3 p2 = tri.vertices[2].position;

    float xmin = (po.x < p1.x) ? ((po.x < p2.x) ? po.x : p2.x) : ((p1.x < p2.x) ? p1.x : p2.x);
    float ymin = (po.y < p1.y) ? ((po.y < p2.y) ? po.y : p2.y) : ((p1.y < p2.y) ? p1.y : p2.y);
    float xmax = (po.x > p1.x) ? ((po.x > p2.x) ? po.x : p2.x) : ((p1.x > p2.x) ? p1.x : p2.x);
    float ymax = (po.y > p1.y) ? ((po.y > p2.y) ? po.y : p2.y) : ((p1.y > p2.y) ? p1.y : p2.y);

    return vec4(xmin, ymin, xmax, ymax);
}

void RasterizeTriangle(Triangle2D tri, FrameBuffer buffer, Texture tex, Shader shader){
    vec4 bb = GetBoundingBox(tri);

    for (int i = bb.y; i < bb.w; i++){
        for (int j = bb.x; j < bb.z; j++){
            if (buffer.IsOOB(vec2(j, i))) continue;

            vec4 bcc = BarycentricCoords(vec2(j, i), tri);
            if (bcc.w > 0.0f && bcc.x > 0.0f && bcc.y > 0.0f && bcc.z > 0.0f && 1.0f - (bcc.x + bcc.y + bcc.z) < 0.001f){
                vec3 z_values = vec3(tri.vertices[0].position.z, tri.vertices[1].position.z, tri.vertices[2].position.z);
                float depth = 1.0f/(
                    (1.0f/z_values.x) * bcc.x + 
                    (1.0f/z_values.y) * bcc.y + 
                    (1.0f/z_values.z) * bcc.z);

                if (depth < buffer.depth_buffer[j][i]){
                    vec3 normal = 
                        (tri.vertices[0].normal/z_values.x * bcc.x +
                        tri.vertices[1].normal/z_values.y * bcc.y +
                        tri.vertices[2].normal/z_values.z * bcc.z) * depth;
                    vec2 uv = 
                        (tri.vertices[0].uv/z_values.x * bcc.x + 
                        tri.vertices[1].uv/z_values.y * bcc.y + 
                        tri.vertices[2].uv/z_values.z * bcc.z) * depth;
                    if (shader.frag != NULL) buffer.render_buffer[j][i] = shader.frag->Evaluate(vertex(vec4(j, i, depth), normal, uv), tex.sample(uv));
                    buffer.depth_buffer[j][i] = depth;
                }
            }
        }
    }
}

void RasterizeLineLow(vec3 a, vec3 b, FrameBuffer buffer){
    int dx = b.x - a.x;
    int dy = b.y - a.y;

    int deltay = 1;
    if (dy < 0){
        deltay = -1;
        dy = -dy;
    }

    int y = a.y;
    int D = 2*dy - dx;
    for (int x = a.x; x < b.x; x++){
        buffer.render_buffer[x][y] = vec3(255.0f, 255.0f, 255.0f);
        if (D > 0){
            y += deltay;
            D += 2*(dy - dx);
        }
        else{
            D += 2*dy;
        }
    }
}

void RasterizeLineHigh(vec3 a, vec3 b, FrameBuffer buffer){
    int dx = b.x - a.x;
    int dy = b.y - a.y;

    int deltax = 1;
    if (dx < 0){
        deltax = -1;
        dx = -dx;
    }

    int x = a.x;
    int D = 2*dx - dy;
    for (int y = a.y; y < b.y; y++){
        buffer.render_buffer[x][y] = vec3(255.0f, 255.0f, 255.0f);
        if (D > 0){
            x += deltax;
            D += 2*(dx - dy);
        }
        else{
            D += 2*dx;
        }
    }
}

void RasterizeLine(vec3 a, vec3 b, FrameBuffer buffer){
    if (abs(b.y - a.y) < abs(b.x - a.x)){
        if (a.x > b.x){
            RasterizeLineLow(b, a, buffer);
        }
        else{
            RasterizeLineLow(a, b, buffer);
        }
    }
    else{
        if (a.y > b.y){
            RasterizeLineHigh(b, a, buffer);
        }
        else{
            RasterizeLineHigh(a, b, buffer);
        }
    }
}