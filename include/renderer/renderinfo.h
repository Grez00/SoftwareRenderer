#ifndef RENDERINFO_H
#define RENDERINFO_H

#include "mat4.h"
#include "texture.h"
#include "framebuffer.h"

typedef struct RenderInfo{
    mat4 transform;
    Texture tex;
    FrameBuffer buffer;
    RenderInfo(mat4 p_transform, Texture p_tex, FrameBuffer p_buffer);
} RenderInfo;

#endif