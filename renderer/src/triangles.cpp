#include "renderer/triangles.h"

// Triangle Constructors

Triangle3D::Triangle3D(vertex v1, vertex v2, vertex v3){
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}
Triangle3D::Triangle3D(){
    vertices[0] = vertex();
    vertices[1] = vertex();
    vertices[2] = vertex();
}

void Triangle3D::ApplyMatrix(mat4 m){
    for (int i = 0; i < 3; i++){
        vertices[i] = vertex(m * vertices[i].position, vertices[i].normal, vertices[i].uv);
    }
}

Triangle2D::Triangle2D(vertex2D v1, vertex2D v2, vertex2D v3){
    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
}
Triangle2D::Triangle2D(){
    vertices[0] = vertex2D();
    vertices[1] = vertex2D();
    vertices[2] = vertex2D();
}

void Triangle2D::ApplyMatrix(mat3 m){
    for (int i = 0; i < 3; i++){
        vertices[i] = vertex2D(m * vertices[i].position, vertices[i].normal, vertices[i].uv);
    }
}