#include "renderer/shader.h"

// VARYINGS

V2F::V2F(){
    world_pos = new vec3[3];
}

// SHADER PARAMS

ShaderParams::ShaderParams(){
    params = std::map<std::string, std::any>();
}

#define SHADERGET(type)                                           \
    if (params.find(name) == params.end()){                       \
        printf("ShaderParams: Error, failed to find %s", name);   \
        return type();                                            \
    }                                                             \
                                                                  \
    std::any result = params[name];                               \
    if (result.has_value()){                                      \
        return std::any_cast<type>(result);                       \
    }                                                             \
    else{                                                         \
        printf("ShaderParams: Error, %s has no value", name);     \
        return type();                                            \
    }                                                             \

int ShaderParams::GetInt(const std::string &name) { SHADERGET(int) }
float ShaderParams::GetFloat(const std::string &name) { SHADERGET(float) }
vec4 ShaderParams::GetVector4(const std::string &name) { SHADERGET(vec4) }
vec3 ShaderParams::GetVector3(const std::string &name) { SHADERGET(vec3) }
mat4 ShaderParams::GetMatrix(const std::string &name) { SHADERGET(mat4) }
Texture ShaderParams::GetTexture(const std::string &name) { SHADERGET(Texture) }
SceneLighting ShaderParams::GetLighting(const std::string &name) { SHADERGET(SceneLighting) }

void ShaderParams::SetInt(const std::string &name, int i){
    params[name] = std::any(i);
}
void ShaderParams::SetFloat(const std::string &name, float f){
    params[name] = std::any(f);
}
void ShaderParams::SetVector4(const std::string &name, vec4 v){
    params[name] = std::any(v);
}
void ShaderParams::SetVector3(const std::string &name, vec3 c){
    params[name] = std::any(c);
}
void ShaderParams::SetMatrix(const std::string &name, mat4 m){
    params[name] = std::any(m);
}
void ShaderParams::SetTexture(const std::string &name, Texture *tex){
    params[name] = std::any(*tex);
}
void ShaderParams::SetLighting(const std::string &name, SceneLighting *lighting_info){
    params[name] = std::any(*lighting_info);
}

// COMBINED SHADER

Shader::Shader(VertexShader *vertex, FragmentShader *frag){
    this->vert = vertex;
    this->frag = frag;
}

Shader::Shader(){
    vert = new VertexShader();
    frag = new FragmentShader();
}

void Shader::SetVertex(VertexShader *vert){
    this->vert = vert;
}

void Shader::SetFragment(FragmentShader *frag){
    this->frag = frag;
}

vec3 Shader::EvaluateFragment(vertex2D v){
    return frag->Evaluate(v, varyings, &params);
}

vertex Shader::EvaluateVertex(vertex v){
    return vert->Evaluate(v, &params);
}

Triangle3D Shader::EvaluateTriangle(Triangle3D tri){
    return vert->EvaluateTriangle(tri, &params, &varyings);
}

// VERTEX SHADERS

VertexShader::VertexShader() {};
vertex VertexShader::Evaluate(vertex v, ShaderParams *params){
    return v;
}
Triangle3D VertexShader::EvaluateTriangle(Triangle3D tri, ShaderParams *params, V2F *V2F){
    return tri;
}

StandardVertex::StandardVertex() {};
vertex StandardVertex::Evaluate(vertex v, ShaderParams *params){
    mat4 model = params->GetMatrix("model");
    mat4 view = params->GetMatrix("view");
    mat4 proj = params->GetMatrix("proj");

    mat4 M_V = view * model;
    mat3 id_M_V = mat3(transpose(inverse(M_V)));
    return vertex(proj * M_V * v.position, id_M_V * v.normal, v.uv);
}
Triangle3D StandardVertex::EvaluateTriangle(Triangle3D tri, ShaderParams *params, V2F *varyings){
    mat4 model = params->GetMatrix("model");
    mat4 view = params->GetMatrix("view");
    mat4 proj = params->GetMatrix("proj");

    varyings->world_pos[0] = vec3(model * tri.vertices[0].position);
    varyings->world_pos[1] = vec3(model * tri.vertices[1].position);
    varyings->world_pos[2] = vec3(model * tri.vertices[2].position);

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
vec3 FragmentShader::Evaluate(vertex2D v, V2F varyings, ShaderParams *params){
    return vec3(1, 0, 1);
}

NormalShader::NormalShader() {}
vec3 NormalShader::Evaluate(vertex2D v, V2F varyings, ShaderParams *params){
    return v.normal;
}

MaterialShader::MaterialShader() {}
vec3 MaterialShader::Evaluate(vertex2D v, V2F varyings, ShaderParams *params){
    vec3 ambient = params->GetVector3("ambient");
    vec3 diffuse = params->GetVector3("diffuse");
    vec3 specular = params->GetVector3("specular");

    return vec3(1, 1, 1) * ambient * diffuse * specular;
}

BlinnPhongShader::BlinnPhongShader() {}
vec3 BlinnPhongShader::Evaluate(vertex2D v, V2F varyings, ShaderParams *params){
    Texture tex = params->GetTexture("tex");
    SceneLighting light_info = params->GetLighting("light_info");
    vec3 cam_pos = params->GetVector3("cam_pos");

    vec3 ambient = params->GetVector3("ambient");
    vec3 diffuse = params->GetVector3("diffuse");
    vec3 specular = params->GetVector3("specular");
    float shininess = params->GetFloat("shininess");

    vec3 tex_col = tex.sample(v.uv);
    vec3 view_dir = normalize(cam_pos - v.position);

    vec3 col = vec3();
    for (int i = 0; i < light_info.num_dir_lights; i++){
        col += light_info.dir_lights[i].Evaluate(v.normal, view_dir, ambient, diffuse, specular, shininess);
    }
    for (int i = 0; i < light_info.num_p_lights; i++){
        col += light_info.p_lights[i].Evaluate(v.normal, view_dir, v.position, ambient, diffuse, specular, shininess);
    }

    return tex_col * col;
}

TextureShader::TextureShader() {}
vec3 TextureShader::Evaluate(vertex2D v, V2F varyings, ShaderParams *params){
    Texture tex = params->GetTexture("tex");

    return tex.sample(v.uv);
}

ColorShader::ColorShader(){}
vec3 ColorShader::Evaluate(vertex2D v, V2F varyings, ShaderParams *params){
    vec3 col = params->GetVector3("col");

    return col;
}