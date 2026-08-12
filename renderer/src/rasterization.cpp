#include "renderer/rasterization.h"

// Triangle Collision and Rasterization

float Edge(vec3 v0, vec3 v1, vec2 p){
    return (p.y - v0.y) * (v1.x - v0.x) - (p.x - v0.x) * (v1.y - v0.y);
}

bool IsTopLeft(vec3 v0, vec3 v1){
    return (v1.y > v0.y) || (v1.x < v0.x);
}

bool IsInTriangle(vec2 p, Triangle2D tri){
    vec3 v0 = tri.vertices[0].position;
    vec3 v1 = tri.vertices[1].position;
    vec3 v2 = tri.vertices[2].position;

    float e0 = Edge(v1, v2, p);
    float e1 = Edge(v2, v0, p);
    float e2 = Edge(v0, v1, p);

    return e0 >= 0.0f && e1 >= 0.0f && e2 >= 0.0f;
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

vec4 GetBoundingBox(Triangle2D tri, int w, int h){
    vec3 po = tri.vertices[0].position;
    vec3 p1 = tri.vertices[1].position;
    vec3 p2 = tri.vertices[2].position;

    int xmin = floor((po.x < p1.x) ? ((po.x < p2.x) ? po.x : p2.x) : ((p1.x < p2.x) ? p1.x : p2.x));
    int ymin = floor((po.y < p1.y) ? ((po.y < p2.y) ? po.y : p2.y) : ((p1.y < p2.y) ? p1.y : p2.y));
    int xmax = ceil((po.x > p1.x) ? ((po.x > p2.x) ? po.x : p2.x) : ((p1.x > p2.x) ? p1.x : p2.x));
    int ymax = ceil((po.y > p1.y) ? ((po.y > p2.y) ? po.y : p2.y) : ((p1.y > p2.y) ? p1.y : p2.y));

    //xmin = std::min(std::max(xmin, 0), w-1);
    //ymin = std::min(std::max(ymin, 0), h-1);
    //xmax = std::min(std::max(xmax, 0), w-1);
    //ymax = std::min(std::max(ymax, 0), h-1);

    return vec4(xmin, ymin, xmax, ymax);
}

void RasterizeTriangle(Triangle2D tri, FrameBuffer *buffer, Shader *shader){
    vec4 bb = GetBoundingBox(tri, buffer->w, buffer->h);

    vec3 v0 = tri.vertices[0].position;
    vec3 v1 = tri.vertices[1].position;
    vec3 v2 = tri.vertices[2].position;
    vec3 z_vals = vec3(1.0f/tri.vertices[0].position.z, 1.0f/tri.vertices[1].position.z, 1.0f/tri.vertices[2].position.z);

    float z_10_diff = z_vals.y - z_vals.x;
    float z_20_diff = z_vals.z - z_vals.x;
    vec2 uv_10_diff = tri.vertices[1].uv - tri.vertices[0].uv;
    vec2 uv_20_diff = tri.vertices[2].uv - tri.vertices[0].uv;
    vec3 normal_10_diff = tri.vertices[1].normal - tri.vertices[0].normal;
    vec3 normal_20_diff = tri.vertices[2].normal - tri.vertices[0].normal;
    vec3 pos_10_diff = shader->world_pos[1] - shader->world_pos[0];
    vec3 pos_20_diff = shader->world_pos[2] - shader->world_pos[0];

    float d_area = 1.0f/Edge(v1, v2, vec2(v0));

    float A01 = v0.y - v1.y, B01 = v1.x - v0.x;
    float A12 = v1.y - v2.y, B12 = v2.x - v1.x;
    float A20 = v2.y - v0.y, B20 = v0.x - v2.x;

    vec2 p = vec2(bb.x + 0.5f, bb.y + 0.5f);
    float w0_row = Edge(v1, v2, p);
    float w1_row = Edge(v2, v0, p);
    float w2_row = Edge(v0, v1, p);

    for (int y = bb.y; y < bb.w; y++){
        float w0 = w0_row;
        float w1 = w1_row;
        float w2 = w2_row;

        for (int x = bb.x; x < bb.z; x++){
            if (w0 >= 0.0f && w1 >= 0.0f && w2 >= 0.0f){
                float b1 = w1*d_area;
                float b2 = w2*d_area;
                float depth = 1.0f / (z_vals.x + z_10_diff*b1 + z_20_diff*b2);
                if (depth < buffer->ReadDepthBuffer(x, y)){
                    b1 = b1*z_vals.y*depth;
                    b2 = b2*z_vals.z*depth;

                    vec2 uv = (tri.vertices[0].uv + uv_10_diff*b1 + uv_20_diff*b2);
                    vec3 normal = (tri.vertices[0].normal + normal_10_diff*b1 + normal_20_diff*b2);
                    vec3 position = (shader->world_pos[0] + pos_10_diff*b1 + pos_20_diff*b2);
                    
                    buffer->SetDepthBuffer(x, y, depth);
                    buffer->SetRenderBuffer(x, y, shader->EvaluateFragment(vertex2D(position, normal, uv)));
                }
            }

            w0 += A12;
            w1 += A20;
            w2 += A01;
        }

        w0_row += B12;
        w1_row += B20;
        w2_row += B01;
    }
}

void RasterizeLineLow(vec3 a, vec3 b, FrameBuffer buffer, vec3 col){
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
        float t = float(x)/b.x;
        float depth = a.z*(1-t) + b.z*t;

        if (depth < buffer.ReadDepthBuffer(x, y)){
            buffer.SetRenderBuffer(x, y, col);
            buffer.SetDepthBuffer(x, y, depth);
        }
        if (D > 0){
            y += deltay;
            D += 2*(dy - dx);
        }
        else{
            D += 2*dy;
        }
    }
}

void RasterizeLineHigh(vec3 a, vec3 b, FrameBuffer buffer, vec3 col){
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
        float t = float(y)/b.y;
        float depth = a.z*(1-t) + b.z*t;

        if (depth < buffer.ReadDepthBuffer(x, y)){
            buffer.SetRenderBuffer(x, y, col);
            buffer.SetDepthBuffer(x, y, depth);
        }
        if (D > 0){
            x += deltax;
            D += 2*(dx - dy);
        }
        else{
            D += 2*dx;
        }
    }
}

void RasterizeLine(vec3 a, vec3 b, FrameBuffer buffer, vec3 col){
    if (abs(b.y - a.y) < abs(b.x - a.x)){
        if (a.x > b.x){
            RasterizeLineLow(b, a, buffer, col);
        }
        else{
            RasterizeLineLow(a, b, buffer, col);
        }
    }
    else{
        if (a.y > b.y){
            RasterizeLineHigh(b, a, buffer, col);
        }
        else{
            RasterizeLineHigh(a, b, buffer, col);
        }
    }
}