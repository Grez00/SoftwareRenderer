#ifndef VERTEX_H
#define VERTEX_H

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"

typedef struct vertex{
    vec4 position;
    vec3 normal;
    vec2 uv;
    vertex(vec4 p_position = vec4(0, 0, 0, 1), vec3 p_normal = vec3(0, 0, 0), vec2 p_uv = vec2(0, 0));
} vertex;

typedef struct vertex2D{
    vec3 position;
    vec3 normal;
    vec2 uv;
    vertex2D(vec3 p_position = vec3(0, 0, 0), vec3 p_normal = vec3(0, 0, 0), vec2 p_uv = vec2(0, 0));
} vertex2D;

#endif