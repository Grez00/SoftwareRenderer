#include "renderer/framebuffer.h"

FrameBuffer::FrameBuffer(int pW, int pH){
    w = pW;
    h = pH;
    
    render_buffer = new vec3*[w];
    depth_buffer = new float*[w];
    for (int i = 0; i < w; i++){
        render_buffer[i] = new vec3[h];
        depth_buffer[i] = new float[h];
    }
}

FrameBuffer::FrameBuffer(){
    w = 0;
    h = 0;

    render_buffer = new vec3*[0];
    depth_buffer = new float*[0];
}

void FrameBuffer::Clear(vec3 clear_col){
    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            render_buffer[j][i] = clear_col;
            depth_buffer[j][i] = 99999999.9f;
        }
    }
}

bool FrameBuffer::IsOOB(vec2 p){
    return p.x < 0 || p.x >= w || p.y < 0 || p.y >= h || !std::isfinite(p.x) || !std::isfinite(p.y);
}