#ifndef MATERIAL_H
#define MATERIAL_H

#include "shader.h"

class Material{
    public:
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;
        Texture *tex;

        Shader *shader;

        Material();
        Material(Shader *shader);

        void Reconstruct();
};

std::map<std::string, Material> ParseMTL(const std::string &filename);

class MaterialStore{
    public:
        MaterialStore();
        Material Get(const std::string &name);
        void LoadMaterials(const std::string &filename);
        friend std::ostream& operator<<(std::ostream &os, const MaterialStore &m);

    private:
        std::map<std::string, Material> materials;
};

#endif