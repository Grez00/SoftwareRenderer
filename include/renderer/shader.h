#ifndef SHADER_H
#define SHADER_H

#include <math.h>

#include "vertex.h"
#include "vectors.h"
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
        FragmentShader *frag;

        Shader();
        Shader(FragmentShader *p_frag);
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

class TextureShader : public FragmentShader{
    public:
        TextureShader();
        vec3 Evaluate(vertex v, vec3 tex_col) override;
};

class ColorShader : public FragmentShader{
    public:
        vec3 col;

        ColorShader(vec3 p_col);
        vec3 Evaluate(vertex v, vec3 tex_col) override; 
};

#endif