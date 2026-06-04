#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <math.h>
#include <cstdint>

#include "vectors.h"

class FrameBuffer{
    private:

    public:
        int w;
        int h;
        uint8_t* render_buffer;
        float* depth_buffer;

        FrameBuffer(int pW, int pH);
        FrameBuffer();
        void SetRenderBuffer(int x, int y, vec3 v);
        void SetDepthBuffer(int x, int y, float d);
        float ReadDepthBuffer(int x, int y);
        void Clear(vec3 clear_col);
        bool IsOOB(vec2 p);
};

#endif