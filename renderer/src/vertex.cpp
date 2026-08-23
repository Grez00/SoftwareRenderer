#include "renderer/vertex.h"

vertex::vertex(vec4 p_position, vec3 p_normal, vec2 p_uv, vec4 p_tangent){
    position = p_position;
    normal = p_normal;
    uv = p_uv;
    tangent = p_tangent;
}

vertex2D::vertex2D(vec3 p_position, vec3 p_normal, vec2 p_uv){
    position = p_position;
    normal = p_normal;
    uv = p_uv;
}