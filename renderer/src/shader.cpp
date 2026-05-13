#include "renderer/shader.h"

FragmentShader::FragmentShader() {};
vec3 FragmentShader::Evaluate(vertex v, vec3 tex_col){
    return vec3(1, 0, 1);
}

Shader::Shader(FragmentShader *p_frag){
    frag = p_frag;
}
Shader::Shader(){
    frag = new FragmentShader();
}

NormalShader::NormalShader() {}
vec3 NormalShader::Evaluate(vertex v, vec3 tex_col){
    return v.normal;
}

BlinnPhongShader::BlinnPhongShader() {}
vec3 BlinnPhongShader::Evaluate(vertex v, vec3 tex_col){
    vec3 light_dir = vec3(0, 1, 0);
    vec3 view_dir = normalize(vec3() - vec3(v.position));
    vec3 normal = normalize(v.normal);
    vec3 half = normalize(light_dir + view_dir);

    vec3 light_color = vec3(0.2, 0.2, 0.2);
    
    vec3 ambient = (tex_col + light_color) * 0.2;
    vec3 diffuse = (tex_col + light_color) * std::max(dot(normal, light_dir), 0.0f);
    vec3 specular = (tex_col + light_color) * std::pow(std::max(dot(normal, half), 0.0f), 2);

    vec3 col = ambient + diffuse + specular;
    return col;
}