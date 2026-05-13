#ifndef TRIANGLES_H
#define TRIANGLES_H

#include "vertex.h"
#include "texture.h"

typedef struct Triangle3D {
    vertex vertices[3];
    Triangle3D(vertex v1, vertex v2, vertex v3);
} Triangle3D;

typedef struct Triangle2D {
    vertex2D vertices[3];
    Triangle2D(vertex2D v1, vertex2D v2, vertex2D v3);
} Triangle2D;

#endif