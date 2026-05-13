#ifndef SHADER_H
#define SHADER_H

#include <math.h>

#include "vertex.h"
#include "vec3.h"
#include "texture.h"
#include "light.h"
#include "helpers.h"

class FragmentShader{
    public:
        FragmentShader();
        virtual vec3 Evaluate(vertex v, vec3 tex_col);
};

class Shader{
    public:
        Shader();
        Shader(FragmentShader *p_frag);
        FragmentShader *frag;
};

class NormalShader : public FragmentShader{
    public:
        NormalShader();
        vec3 Evaluate(vertex v, vec3 tex_col) override;
};

class BlinnPhongShader : public FragmentShader{
    public:
        BlinnPhongShader();
        vec3 Evaluate(vertex v, vec3 tex_col) override;
};

#endif