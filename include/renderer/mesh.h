#ifndef MESH_H
#define MESH_H

#include <iostream>
#include <fstream>
#include <vector>

#include "vertex.h"
#include "texture.h"
#include "helpers.h"
#include "geometry.h"
#include "shader.h"

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
        ShaderStore *shaders;
        mat4 matrix;
        std::map<int, std::string> index_to_mat;

        Model();
        Model(const std::string &filename);
        Model(Mesh *mesh);
        Model(Mesh *mesh, ShaderStore *mats, mat4 model);
        void LinkMaterials(ShaderStore *mats);
        void LinkMaterial(Shader *shader, const std::string &name);
        void LinkMatrix(mat4 matrix);
};

#endif