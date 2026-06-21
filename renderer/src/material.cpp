#include "renderer/material.h"

Material::Material() {}
Material::Material(Shader *shader){
    this->shader = shader;
}