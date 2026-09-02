#ifndef LIGHT_H
#define LIGHT_H

#include "vectors.h"
#include "mat3.h"

struct dirlight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    dirlight();
    dirlight(vec3 dir, vec3 ambient, vec3 diffuse, vec3 specular);
    vec3 Evaluate(vec3 normal, vec3 view_dir, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic);
    vec3 EvaluateTangentSpace(vec3 normal, vec3 light_dir, vec3 view_dir, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic);
};

struct pointlight{
    vec3 position;

    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    pointlight();
    pointlight(vec3 pos, float linear, float quadratic, vec3 ambient, vec3 diffuse, vec3 specular);
    vec3 Evaluate(vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic);
    vec3 EvaluateTangentSpace(vec3 normal, vec3 light_dir, vec3 view_dir, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic);
};

class SceneLighting{
    public:
        dirlight *dir_lights;
        pointlight *p_lights;

        int num_dir_lights;
        int num_p_lights;
        int num_lights;

        // Vertex to fragment light attributes
        //vec3 **tangent_light_dir;

        // Interpolated light attributes
        //vec3 *interp_light_dir;

        SceneLighting();
        SceneLighting(dirlight *dir_lights, pointlight *p_lights, int num_dir_lights, int num_p_lights);

        void CalculateTangentLightDir(vec3 **tangent_light_dir, vec3 pos, mat3 TBN, int i);
        void InterpolateLightDir(vec3 **tangent_light_dir, vec3 *interp_light_dir, vec2 uv);
};

#endif