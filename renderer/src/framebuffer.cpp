#include "renderer/framebuffer.h"

FrameBuffer::FrameBuffer(int pW, int pH){
    w = pW;
    h = pH;
    
    render_buffer = new uint8_t[w*h*3];
    depth_buffer = new float[w*h];
}

FrameBuffer::FrameBuffer(){
    w = 0;
    h = 0;

    render_buffer = new uint8_t[0];
    depth_buffer = new float[0];
}

void FrameBuffer::SetRenderBuffer(int x, int y, vec3 v){
    int index = (y*w + x)*3;
    render_buffer[index] = static_cast<uint8_t>(std::min(std::max(v.x, 0.0f), 1.0f) * 255.0f);
    render_buffer[index+1] = static_cast<uint8_t>(std::min(std::max(v.y, 0.0f), 1.0f) * 255.0f);
    render_buffer[index+2] = static_cast<uint8_t>(std::min(std::max(v.z, 0.0f), 1.0f) * 255.0f);
}

void FrameBuffer::SetDepthBuffer(int x, int y, float d){
    depth_buffer[y*w + x] = d;
}

float FrameBuffer::ReadDepthBuffer(int x, int y){
    return depth_buffer[y*w + x];
}

void FrameBuffer::Clear(vec3 clear_col){
    uint8_t clear_col_int[3] = {
        static_cast<uint8_t>(clear_col.x * 255.0f),
        static_cast<uint8_t>(clear_col.y * 255.0f),
        static_cast<uint8_t>(clear_col.z * 255.0f)
    };  
    
    int num_pixels = w * h;
    for (int i = 0; i < num_pixels; i++){
        render_buffer[i*3] = clear_col_int[0];
        render_buffer[i*3+1] = clear_col_int[1];
        render_buffer[i*3+2] = clear_col_int[2];
        depth_buffer[i] = 99999999.9f;
    }
}

bool FrameBuffer::IsOOB(vec2 p){
    return p.x < 0 || p.x >= w || p.y < 0 || p.y >= h || !std::isfinite(p.x) || !std::isfinite(p.y);
}