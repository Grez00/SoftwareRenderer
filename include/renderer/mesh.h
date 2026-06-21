#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <vector>

#include "vertex.h"
#include "texture.h"
#include "material.h"
#include "helpers.h"
#include "geometry.h"

class Mesh{
    public:
        vec4 *positions;
        vec3 *normals;
        vec2 *uvs;
        vec3 *indices;

        int vert_count;
        int index_count;

        Mesh(vec4 *p_positions, vec3 *normals, vec2 *uvs, vec3 *p_indices, int p_vertcount, int p_indexcount);
        Mesh(const std::string &filename);
        Mesh();
        sphere GetBoundingSphere();
};

class Model{
    public:
        Mesh *mesh;
        Material *mat;
        mat4 model;

        Model();
        Model(Mesh *mesh);
        Model(Mesh *mesh, Material *mat, mat4 model);
        void LinkMaterial(Material *p_mat);
        void LinkMatrix(mat4 model);
};

#endif