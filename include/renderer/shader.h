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

class ShaderParams{
    public:
        ShaderParams();

        int GetInt(const std::string &name);
        float GetFloat(const std::string &name);
        vec4 GetVector4(const std::string &name);
        vec3 GetVector3(const std::string &name);
        mat4 GetMatrix(const std::string &name);
        Texture GetTexture(const std::string &name);
        SceneLighting GetLighting(const std::string &name);

        void SetInt(const std::string &name, int i);
        void SetFloat(const std::string &name, float f);
        void SetVector4(const std::string &name, vec4 v);
        void SetVector3(const std::string &name, vec3 c);
        void SetMatrix(const std::string &name, mat4 m);
        void SetTexture(const std::string &name, Texture *tex);
        void SetLighting(const std::string &name, SceneLighting *lighting_info);

    private:
        std::map<std::string, std::any> params;
};

class FragmentShader{
    public:
        FragmentShader();
        virtual vec3 Evaluate(vertex2D v, V2F varyings, ShaderParams *params);
};

class VertexShader{
    public:
        VertexShader();
        virtual vertex Evaluate(vertex v, ShaderParams *params);
        virtual Triangle3D EvaluateTriangle(Triangle3D tri, ShaderParams *params, V2F *V2F);
};

class Shader{
    public:
        ShaderParams params;
        V2F varyings;

        Shader();
        Shader(VertexShader *vert, FragmentShader *frag);

        void SetVertex(VertexShader *vert);
        void SetFragment(FragmentShader *frag);

        vec3 EvaluateFragment(vertex2D v);
        vertex EvaluateVertex(vertex v);
        Triangle3D EvaluateTriangle(Triangle3D tri);

    private:
        VertexShader *vert;
        FragmentShader *frag;
};

class StandardVertex : public VertexShader{
    public:
        StandardVertex();
        vertex Evaluate(vertex v, ShaderParams *params) override;
        Triangle3D EvaluateTriangle(Triangle3D tri, ShaderParams *params, V2F *V2F) override;
};

class NormalShader : public FragmentShader{
    public:
        NormalShader();
        vec3 Evaluate(vertex2D v, V2F varyings, ShaderParams *params) override;
};

class MaterialShader : public FragmentShader {
    public:
        MaterialShader();
        vec3 Evaluate(vertex2D v, V2F varyings, ShaderParams *params) override;
};

class BlinnPhongShader : public FragmentShader{
    public:
        BlinnPhongShader();
        vec3 Evaluate(vertex2D v, V2F varyings, ShaderParams *params) override;
};

class TextureShader : public FragmentShader{
    public:
        TextureShader();
        vec3 Evaluate(vertex2D v, V2F varyings, ShaderParams *params) override;
};

class ColorShader : public FragmentShader{
    public:
        ColorShader();
        vec3 Evaluate(vertex2D v, V2F varyings, ShaderParams *params) override; 
};

#endif