#include "renderer/light.h"

dirlight::dirlight() {}
dirlight::dirlight(vec3 dir, vec3 ambient, vec3 diffuse, vec3 specular){
    this->direction = normalize(dir);
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
}
vec3 dirlight::Evaluate(vec3 normal, vec3 view_dir, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic){
    vec3 half = normalize(direction + view_dir);
    normal = normalize(normal);

    float spec = std::max(dot(normal, half), 0.0f);
    float diff = std::max(dot(normal, direction), 0.0f);
    vec3 col = (
        ambient * m_diffuse + 
        diffuse * diff * m_diffuse + 
        specular * m_specular * pow(spec, m_smoothness)
    );
    return col;
}
vec3 dirlight::EvaluateTangentSpace(vec3 normal, vec3 light_dir, vec3 view_dir, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic){
    vec3 half = normalize(light_dir + view_dir);
    normal = normalize(normal);

    float spec = std::max(dot(normal, half), 0.0f);
    float diff = std::max(dot(normal, light_dir), 0.0f);
    vec3 col = (
        ambient * m_diffuse + 
        diffuse * diff * m_diffuse + 
        specular * m_specular * pow(spec, m_smoothness)
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
vec3 pointlight::Evaluate(vec3 normal, vec3 view_dir, vec3 frag_pos, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic){
    vec3 direction = position - frag_pos;
    float distance = length(direction);
    direction = normalize(direction);

    vec3 half = normalize(direction + view_dir);
    normal = normalize(normal);

    float attenuation = 1.0f / (1.0f + linear * distance + quadratic * distance*distance);

    float diff = std::max(dot(normal, direction), 0.0f);
    float spec = std::max(dot(normal, half), 0.0f);

    vec3 col = 
        (ambient * m_diffuse + 
        (diffuse * diff) * m_diffuse + 
        (specular * pow(spec, m_smoothness) * m_specular)) * attenuation;
    return col;
}
vec3 pointlight::EvaluateTangentSpace(vec3 normal, vec3 light_dir, vec3 view_dir, vec3 m_diffuse, vec3 m_specular, float m_smoothness, float m_metallic){
    float distance = length(light_dir);
    light_dir = normalize(light_dir);

    vec3 half = normalize(light_dir + view_dir);
    normal = normalize(normal);

    float attenuation = 1.0f / (1.0f + linear * distance + quadratic * distance*distance);

    float diff = std::max(dot(normal, light_dir), 0.0f);
    float spec = std::max(dot(normal, half), 0.0f);
    vec3 col = 
        (ambient * m_diffuse + 
        diffuse * diff * m_diffuse + 
        (specular * pow(spec, m_smoothness) * m_specular)) * attenuation;
    return col;
}

SceneLighting::SceneLighting(){
    num_dir_lights = 0;
    num_p_lights = 0;
    num_lights = 0;

    this->tangent_light_dir = new vec3*[num_lights];
    for (int i = 0; i < num_lights; i++){
        this->tangent_light_dir[i] = new vec3[3];
    }
    this->interp_light_dir = new vec3[num_lights];
}
SceneLighting::SceneLighting(dirlight *dir_lights, pointlight *p_lights, int num_dir_lights, int num_p_lights){
    this->dir_lights = dir_lights;
    this->p_lights = p_lights;
    this->num_dir_lights = num_dir_lights;
    this->num_p_lights = num_p_lights;
    this->num_lights = num_dir_lights + num_p_lights;

    this->tangent_light_dir = new vec3*[num_lights];
    for (int i = 0; i < num_lights; i++){
        this->tangent_light_dir[i] = new vec3[3];
    }
    this->interp_light_dir = new vec3[num_lights];
}
void SceneLighting::CalculateTangentLightDir(vec3 pos, mat3 TBN, int i){
    for (int j = 0; j < num_dir_lights; j++){
        tangent_light_dir[j][i] = TBN * dir_lights[j].direction;
    }
    for (int j = num_dir_lights; j < num_lights; j++){
        tangent_light_dir[j][i] = TBN * (p_lights[j-num_dir_lights].position - pos);
    }
}
void SceneLighting::InterpolateLightDir(vec2 uv){
    for (int i = 0; i < num_lights; i++){
        interp_light_dir[i] = 
            tangent_light_dir[i][0] + 
            (tangent_light_dir[i][1] - tangent_light_dir[i][0])*uv.x + 
            (tangent_light_dir[i][2] - tangent_light_dir[i][0])*uv.y;
    }
}