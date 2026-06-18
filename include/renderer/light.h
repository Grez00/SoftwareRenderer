#ifndef LIGHT_H
#define LIGHT_H

#include "vectors.h"

struct dirlight{
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    dirlight(vec3 dir, vec3 ambient, vec3 diffuse, vec3 specular);
    vec3 Evaluate(vec3 normal, vec3 view_dir);
};

struct pointlight{
    vec3 position;

    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    pointlight(vec3 pos, float linear, float quadratic, vec3 ambient, vec3 diffuse, vec3 specular);
    vec3 Evaluate(vec3 normal, vec3 view_dir, vec3 frag_pos);
};

class SceneLighting{
    public:
        dirlight *dir_lights;
        pointlight *p_lights;

        int num_dir_lights;
        int num_p_lights;

        SceneLighting(dirlight *dir_lights, pointlight *p_lights, int num_dir_lights, int num_p_lights);
};

#endif