#ifndef MATERIAL_H
#define MATERIAL

#include "shader.h"

class Material{
    public:
        Shader *shader;
        Material();
        Material(Shader *shader);
};

#endif