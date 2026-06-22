#ifndef SHADER_H
#define SHADER_H

#include <math.h>

#include "vertex.h"
#include "vectors.h"
#include "texture.h"
#include "light.h"
#include "helpers.h"
#include "mat4.h"
#include "mat3.h"
#include "triangles.h"

class V2F{
    public:
        vec3 *world_pos;
        V2F();
};

class FragmentShader{
    public:
        SceneLighting *light_info;
        vec3 cam_pos;

        FragmentShader();
        virtual vec3 Evaluate(vertex2D v);
};

class VertexShader{
    public:
        V2F varyings;

        VertexShader();
        virtual vertex Evaluate(vertex v);
        virtual Triangle3D EvaluateTriangle(Triangle3D tri);
};

class Shader{
    public:
        VertexShader *vertex;
        FragmentShader *frag;

        Shader();
        Shader(VertexShader *vertex, FragmentShader *frag);
};

class StandardVertex : public VertexShader{
    public:
        mat4 model;
        mat4 view;
        mat4 proj;

        StandardVertex();
        StandardVertex(mat4 model, mat4 view, mat4 proj);
        vertex Evaluate(vertex v) override;
        Triangle3D EvaluateTriangle(Triangle3D tri) override;
};

class NormalShader : public FragmentShader{
    public:
        NormalShader();
        vec3 Evaluate(vertex2D v) override;
};

class BlinnPhongShader : public FragmentShader{
    public:
        Texture *tex;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;

        BlinnPhongShader();
        BlinnPhongShader(Texture *tex, SceneLighting *light_info, vec3 cam_pos);
        vec3 Evaluate(vertex2D v) override;
};

class TextureShader : public FragmentShader{
    public:
        Texture *tex;

        TextureShader();
        TextureShader(Texture *tex);
        vec3 Evaluate(vertex2D v) override;
};

class ColorShader : public FragmentShader{
    public:
        vec3 col;

        ColorShader(vec3 col);
        vec3 Evaluate(vertex2D v) override; 
};

#endif