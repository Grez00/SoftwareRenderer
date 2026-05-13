#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include <math.h>

#include "vec2.h"
#include "vec3.h"

class FrameBuffer{
    private:

    public:
        int w;
        int h;
        vec3** render_buffer;
        float** depth_buffer;

        FrameBuffer(int pW, int pH);
        FrameBuffer();
        void Clear(vec3 clear_col);
        bool IsOOB(vec2 p);
};

#endif