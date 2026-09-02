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
        vec3 *frag_pos;
        vec3 *tangent_view_dir;
        vec3 interp_view_dir;

        vec3 **tangent_light_dir;
        vec3 *interp_light_dir;

        V2F();
        V2F(int num_lights);
};

class Shader{
    public:
        // Scene info
        SceneLighting *light_info;
        vec3 cam_pos;

        // Matrices
        mat4 model;
        mat4 view;
        mat4 proj;

        // Vertex to Fragment attributes
        //vec3 *frag_pos;
        //vec3 *tangent_view_dir;

        // Interpolated attributes
        //vec3 interp_view_dir;

        Shader();

        virtual vec3 EvaluateFragment(vertex2D v, V2F *v2f);
        virtual vertex EvaluateVertex(vertex v, V2F *v2f, int i);
        virtual Triangle3D EvaluateTriangle(Triangle3D tri, V2F *v2f);
};

class NormalShader : public Shader{
    public:
        NormalShader();

        vec3 EvaluateFragment(vertex2D v, V2F *v2f) override;
};

class MaterialShader : public Shader {
    public:
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;
        float shininess;

        MaterialShader();
        MaterialShader(vec3 ambient, vec3 diffuse, vec3 specular, float shininess);

        vec3 EvaluateFragment(vertex2D v, V2F *v2f) override;
};

class BlinnPhongShader : public Shader{
    public:
        vec3 tint;
        float metallic;
        float smoothness;

        Texture *map_albedo;
        Texture *map_roughness;
        Texture *map_metallic;
        Texture *map_normal;

        BlinnPhongShader();
        BlinnPhongShader(Texture *p_albedo);
        BlinnPhongShader(Texture *p_albedo, Texture *p_roughness, Texture *p_metallic, Texture *p_normal);
        BlinnPhongShader(vec3 tint, float metallic, float smoothness);

        vec3 EvaluateFragment(vertex2D v, V2F *v2f) override;
};

class TextureShader : public Shader{
    public:
        Texture *tex;

        TextureShader();
        TextureShader(Texture *tex);

        vec3 EvaluateFragment(vertex2D v, V2F *v2f) override;
};

class ColorShader : public Shader{
    public:
        vec3 col;

        ColorShader();
        ColorShader(vec3 col);

        vec3 EvaluateFragment(vertex2D v, V2F *v2f) override;
};

class SkyboxShader : public Shader{
    public:
        CubeMap *cubemap;

        SkyboxShader();
        SkyboxShader(CubeMap *cubemap);

        vertex EvaluateVertex(vertex v, V2F *v2f, int i) override;
        Triangle3D EvaluateTriangle(Triangle3D tri, V2F *v2f) override;
        vec3 EvaluateFragment(vertex2D v, V2F *v2f) override;
};

class ShaderStore{
    public:
        int num_lights;

        ShaderStore();
        ShaderStore(std::map<std::string, Shader*> shaders);
        Shader* Get(const std::string &name);
        void LoadShaders(const std::string &filename);
        void Add(Shader *shader, const std::string &name);
        void SetSceneInfo(mat4 model, mat4 view, mat4 proj, vec3 cam_pos, SceneLighting *light_info);
        friend std::ostream& operator<<(std::ostream &os, const ShaderStore &m);

    private:
        std::map<std::string, Shader*> shaders;
};

std::map<std::string, Shader*> ParseMTL(const std::string &filename);

#endif