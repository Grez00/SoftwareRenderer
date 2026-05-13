#include "renderer/renderinfo.h"

RenderInfo::RenderInfo(mat4 p_transform, Texture p_tex, FrameBuffer p_buffer){
    transform = p_transform;
    tex = p_tex;
    buffer = p_buffer;
}