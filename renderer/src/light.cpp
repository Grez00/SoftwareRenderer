#include "renderer/light.h"

dirlight::dirlight() {}
dirlight::dirlight(vec3 dir, vec3 ambient, vec3 diffuse, vec3 specular){
    this->direction = normalize(dir);
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
}
vec3 dirlight::Evaluate(vec3 normal, vec3 view_dir, vec3 m_ambient, vec3 m_diffuse, vec3 m_specular, float m_shininess){
    vec3 half = normalize(direction + view_dir);
    normal = normalize(normal);

    float spec = std::max(dot(normal, half), 0.0f);
    float diff = std::max(dot(normal, direction), 0.0f);
    vec3 col = (
        ambient * m_ambient + 
        diffuse * diff * m_diffuse + 
        specular * m_specular * pow(spec, m_shininess)
    );
    return col;
}

pointlight::pointlight() {}
pointlight::pointlight(vec3 pos, float linear, float quadratic, vec3 ambient, vec3 diffuse, vec3 specular){
    this->position = pos;
    this->linear = linear;
    this->quadratic = quadratic;
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
}
vec3 pointlight::Evaluate(vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 m_ambient, vec3 m_diffuse, vec3 m_specular, float m_shininess){
    vec3 direction = position - frag_pos;
    float distance = length(direction);
    direction = normalize(direction);

    vec3 half = normalize(direction + view_dir);
    normal = normalize(normal);

    float attenuation = 1.0f / (1.0f + linear * distance + quadratic * distance*distance);
    float spec = std::max(dot(normal, half), 0.0f);

    vec3 col = 
        (ambient * m_ambient + 
        (diffuse * std::max(dot(normal, direction), 0.0f)) * m_diffuse + 
        (specular * pow(spec, m_shininess) * m_specular)) * attenuation;
    return col;
}

SceneLighting::SceneLighting(){
    num_dir_lights = 0;
    num_p_lights = 0;
}
SceneLighting::SceneLighting(dirlight *dir_lights, pointlight *p_lights, int num_dir_lights, int num_p_lights){
    this->dir_lights = dir_lights;
    this->p_lights = p_lights;
    this->num_dir_lights = num_dir_lights;
    this->num_p_lights = num_p_lights;
}