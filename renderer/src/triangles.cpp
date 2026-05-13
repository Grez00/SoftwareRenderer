#include "renderer/triangles.h"

// Triangle Constructors

Triangle3D::Triangle3D(vertex v1, vertex v2, vertex v3){
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}

Triangle2D::Triangle2D(vertex2D v1, vertex2D v2, vertex2D v3){
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}