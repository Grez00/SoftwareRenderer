#ifndef LIGHT_H
#define LIGHT_H

#include "vectors.h"

struct dirlight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    dirlight();
    dirlight(vec3 dir, vec3 ambient, vec3 diffuse, vec3 specular);
    vec3 Evaluate(vec3 normal, vec3 view_dir, vec3 m_ambient, vec3 m_diffuse, vec3 m_specular, float m_shininess);
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
    vec3 Evaluate(vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 m_ambient, vec3 m_diffuse, vec3 m_specular, float m_shininess);
};

class SceneLighting{
    public:
        dirlight *dir_lights;
        pointlight *p_lights;

        int num_dir_lights;
        int num_p_lights;

        SceneLighting();
        SceneLighting(dirlight *dir_lights, pointlight *p_lights, int num_dir_lights, int num_p_lights);
};

#endif