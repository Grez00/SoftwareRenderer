#include "renderer/shader.h"

// VARYINGS

V2F::V2F(){
    world_pos = new vec3[3];
}

// COMBINED SHADER

Shader::Shader(VertexShader *vertex, FragmentShader *frag){
    this->vertex = vertex;
    this->frag = frag;
}
Shader::Shader(){
    vertex = new VertexShader();
    frag = new FragmentShader();
}

// VERTEX SHADERS

VertexShader::VertexShader() {};
vertex VertexShader::Evaluate(vertex v){
    return v;
}
Triangle3D VertexShader::EvaluateTriangle(Triangle3D tri){
    return tri;
}

StandardVertex::StandardVertex() {
    varyings = V2F();
};
vertex StandardVertex::Evaluate(vertex v){
    mat4 M_V = view * model;
    mat3 id_M_V = mat3(transpose(inverse(M_V)));
    return vertex(proj * M_V * v.position, id_M_V * v.normal, v.uv);
}
Triangle3D StandardVertex::EvaluateTriangle(Triangle3D tri){
    varyings.world_pos[0] = vec3(model * tri.vertices[0].position);
    varyings.world_pos[1] = vec3(model * tri.vertices[1].position);
    varyings.world_pos[2] = vec3(model * tri.vertices[2].position);

    mat4 M_V = view * model;
    mat4 M_V_P = proj * M_V;
    mat3 id_M = mat3(transpose(inverse(model)));

    return Triangle3D(
        vertex(M_V_P * tri.vertices[0].position, id_M * tri.vertices[0].normal, tri.vertices[0].uv),
        vertex(M_V_P * tri.vertices[1].position, id_M * tri.vertices[1].normal, tri.vertices[1].uv),
        vertex(M_V_P * tri.vertices[2].position, id_M * tri.vertices[2].normal, tri.vertices[2].uv)
    );
}

// FRAGMENT SHADERS

FragmentShader::FragmentShader() {};
vec3 FragmentShader::Evaluate(vertex2D v){
    return vec3(1, 0, 1);
}

NormalShader::NormalShader() {}
vec3 NormalShader::Evaluate(vertex2D v){
    return v.normal;
}

BlinnPhongShader::BlinnPhongShader() {}
BlinnPhongShader::BlinnPhongShader(Texture *tex, SceneLighting *light_info, vec3 cam_pos){
    this->tex = tex;
    this->light_info = light_info;
    this->cam_pos = cam_pos;
}
vec3 BlinnPhongShader::Evaluate(vertex2D v){
    vec3 tex_col = vec3(1, 0, 1);
    if (tex != NULL) tex_col = tex->sample(v.uv);
    vec3 view_dir = normalize(cam_pos - v.position);

    vec3 col = vec3();
    for (int i = 0; i < light_info->num_dir_lights; i++){
        col += light_info->dir_lights[i].Evaluate(v.normal, view_dir);
    }
    for (int i = 0; i < light_info->num_p_lights; i++){
        col += light_info->p_lights[i].Evaluate(v.normal, view_dir, v.position);
    }

    return tex_col * col;
}

TextureShader::TextureShader() {}
TextureShader::TextureShader(Texture *tex) {
    this->tex = tex;
}
vec3 TextureShader::Evaluate(vertex2D v){
    if (tex != NULL) return tex->sample(v.uv);
    else return vec3(1, 0, 1);
}

ColorShader::ColorShader(vec3 col){
    this->col = col;
}
vec3 ColorShader::Evaluate(vertex2D v){
    return col;
}