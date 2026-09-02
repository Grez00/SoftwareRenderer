#include "renderer/shader.h"

V2F::V2F() {
    frag_pos = new vec3[3];
    tangent_view_dir = new vec3[3];
}
V2F::V2F(int num_lights){
    frag_pos = new vec3[3];
    tangent_view_dir = new vec3[3];

    tangent_light_dir = new vec3*[num_lights];
    for (int i = 0; i < num_lights; i++){
        tangent_light_dir[i] = new vec3[3];
    }
    interp_light_dir = new vec3[num_lights];
}

Shader::Shader() {
    //frag_pos = new vec3[3];
    //tangent_view_dir = new vec3[3];
}

vec3 Shader::EvaluateFragment(vertex2D v, V2F *v2f){
    return vec3(1, 0, 1);
}

vertex Shader::EvaluateVertex(vertex v, V2F *v2f, int i){
    mat4 M_V = view * model;
    mat3 id_M = mat3(transpose(inverse(M_V)));

    v2f->frag_pos[i] = vec3(model * v.position);
    //frag_pos[i] = vec3(model * v.position);

    vec3 normal = id_M * v.normal;
    vec3 tangent = id_M * vec3(v.tangent);
    vec3 bitangent = cross(normal, tangent) * v.tangent.w;
    mat3 TBN = mat3(
        tangent.x, tangent.y, tangent.z,
        bitangent.x, bitangent.y, bitangent.z,
        normal.x, normal.y, normal.z
    );

    light_info->CalculateTangentLightDir(v2f->tangent_light_dir, v2f->frag_pos[i], TBN, i);
    v2f->tangent_view_dir[i] = TBN * (cam_pos - v2f->frag_pos[i]);
    //light_info->CalculateTangentLightDir(frag_pos[i], TBN, i);
    //tangent_view_dir[i] = TBN * (cam_pos - frag_pos[i]);

    return vertex(proj * M_V * v.position, normal, v.uv);
}

Triangle3D Shader::EvaluateTriangle(Triangle3D tri, V2F *v2f){
    mat4 M_V = view * model;
    mat4 M_V_P = proj * M_V;
    mat3 id_M = mat3(transpose(inverse(model)));

    Triangle3D result = Triangle3D();

    for (int i = 0; i < 3; i++){
        v2f->frag_pos[i] = vec3(model * tri.vertices[i].position);
        //frag_pos[i] = vec3(model * tri.vertices[i].position);

        vec3 normal = id_M * tri.vertices[i].normal;
        vec3 tangent = id_M * vec3(tri.vertices[i].tangent);
        vec3 bitangent = cross(normal, tangent) * tri.vertices[i].tangent.w;
        mat3 TBN = mat3(
            tangent.x, tangent.y, tangent.z,
            bitangent.x, bitangent.y, bitangent.z,
            normal.x, normal.y, normal.z
        );

        light_info->CalculateTangentLightDir(v2f->tangent_light_dir, v2f->frag_pos[i], TBN, i);
        v2f->tangent_view_dir[i] = TBN * (cam_pos - v2f->frag_pos[i]);
        //light_info->CalculateTangentLightDir(frag_pos[i], TBN, i);
        //tangent_view_dir[i] = TBN * (cam_pos - frag_pos[i]);

        result.vertices[i].position = M_V_P * tri.vertices[i].position;
        result.vertices[i].normal = normal;
        result.vertices[i].uv = tri.vertices[i].uv;
    }

    return result;
}

NormalShader::NormalShader() {}
vec3 NormalShader::EvaluateFragment(vertex2D v, V2F *v2f){
    return v.normal;
}

MaterialShader::MaterialShader() {}
MaterialShader::MaterialShader(vec3 ambient, vec3 diffuse, vec3 specular, float shininess){
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;
}
vec3 MaterialShader::EvaluateFragment(vertex2D v, V2F *v2f){
    return vec3(1, 1, 1) * ambient * diffuse * specular;
}

BlinnPhongShader::BlinnPhongShader() {
    this->map_albedo = new Texture();
    this->map_roughness = new Texture();
    this->map_metallic = new Texture();
    this->map_normal = new Texture();
}
BlinnPhongShader::BlinnPhongShader(Texture *p_albedo){
    this->map_albedo = p_albedo;
    this->map_roughness = new Texture();
    this->map_metallic = new Texture();
    this->map_normal = new Texture();
}
BlinnPhongShader::BlinnPhongShader(Texture *p_albedo, Texture *p_roughness, Texture *p_metallic, Texture *p_normal){
    this->map_albedo = p_albedo;
    this->map_roughness = p_roughness;
    this->map_metallic = p_metallic;
    this->map_normal = p_normal;
}
BlinnPhongShader::BlinnPhongShader(vec3 tint, float metallic, float smoothness){
    this->tint = tint;
    this->metallic = metallic;
    this->smoothness = smoothness;
}
vec3 BlinnPhongShader::EvaluateFragment(vertex2D v, V2F *v2f){
    vec3 albedo = tint;
    if (!map_albedo->IsEmpty()) albedo = map_albedo->sample(v.uv);

    float smoothness_value = smoothness;
    if (!map_roughness->IsEmpty()) smoothness_value = 1.0f - length(map_roughness->sample(v.uv));

    float metallic_value = metallic;
    if (!map_metallic->IsEmpty()) metallic_value = length(map_metallic->sample(v.uv));

    vec3 specular = albedo * metallic_value;
    albedo *= 1.0f - metallic_value;

    vec3 normal = v.normal;
    vec3 col = vec3();
    if (!map_normal->IsEmpty()){
        normal = map_normal->sample(v.uv);

        for (int i = 0; i < light_info->num_dir_lights; i++){
            col += light_info->dir_lights[i].EvaluateTangentSpace(normal, v2f->interp_light_dir[i], v2f->interp_view_dir, albedo, specular, smoothness_value * 100.0f, metallic_value);
        }
        for (int i = light_info->num_dir_lights; i < light_info->num_lights; i++){
            col += light_info->p_lights[i-light_info->num_dir_lights].EvaluateTangentSpace(normal, v2f->interp_light_dir[i], v2f->interp_view_dir, albedo, specular, smoothness_value * 100.0f, metallic_value);
        }
    }
    else{
        vec3 view_dir = normalize(cam_pos - v.position);

        for (int i = 0; i < light_info->num_dir_lights; i++){
            col += light_info->dir_lights[i].Evaluate(normal, view_dir, albedo, specular, smoothness_value * 100.0f, metallic_value);
        }
        for (int i = 0; i < light_info->num_p_lights; i++){
            col += light_info->p_lights[i].Evaluate(normal, view_dir, v.position, albedo, specular, smoothness_value * 100.0f, metallic_value);
        }
    }

    return col;
}

TextureShader::TextureShader() {}
TextureShader::TextureShader(Texture *tex){
    this->tex = tex;
}
vec3 TextureShader::EvaluateFragment(vertex2D v, V2F *v2f){
    return tex->sample(v.uv);
}

ColorShader::ColorShader() {
    col = vec3(1, 0, 1);
}
ColorShader::ColorShader(vec3 col){
    this->col = col;
}
vec3 ColorShader::EvaluateFragment(vertex2D v, V2F *v2f){
    return col;
}

SkyboxShader::SkyboxShader() {}
SkyboxShader::SkyboxShader(CubeMap *cubemap) {
    this->cubemap = cubemap;
}
vertex SkyboxShader::EvaluateVertex(vertex v, V2F *v2f, int i){
    return vertex(proj * view * v.position, vec3(v.position), v.uv);
}
Triangle3D SkyboxShader::EvaluateTriangle(Triangle3D tri, V2F *v2f){
    Triangle3D result = Triangle3D();

    for (int i = 0; i < 3; i++){
        vertex v = tri.vertices[i];
        vec4 pos = proj * view * v.position;
        result.vertices[i] = vertex(pos, v.position, v.uv);
    }

    return result;
}
vec3 SkyboxShader::EvaluateFragment(vertex2D v, V2F *v2f){
    return cubemap->sample(v.normal);
}

// SHADER STORE

std::map<std::string, Shader*> ParseMTL(const std::string &filename){
    std::map<std::string, Shader*> output;

    std::string file_end = ".mtl";
    if (filename.length() < file_end.length() || filename.compare(filename.length() - file_end.length(), file_end.length(), file_end)){
        printf("ParseMTL: Error, invalid file type (got: %s)\n", filename.c_str());
        return output;
    }

    std::string line;
    std::ifstream file(filename);

    if (!file.is_open()){
        printf("ParseMTL: Error, failed to open file, filename: %s\n", filename.c_str());
        return output;
    }

    std::string texture_path = "assets/images/";

    BlinnPhongShader *current;
    while(getline(file, line)){
        std::vector<std::string> tokens = split(line, " ");

        if (tokens[0] == "newmtl"){
            current = new BlinnPhongShader();
            current->map_albedo = new Texture();
            current->map_roughness = new Texture();
            current->map_metallic = new Texture();
            current->map_normal = new Texture();
            output[tokens[1]] = current;
        }
        else if (tokens[0] == "Pr"){
            current->smoothness = std::stof(tokens[1]);
        }
        else if (tokens[0] == "Pm"){
            current->metallic = std::stof(tokens[1]);
        }
        else if (tokens[0] == "Kd"){
            current->tint = vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));;
        }
        else if (tokens[0] == "map_Kd"){
            current->map_albedo = new Texture(texture_path + tokens[1]);
        }
        else if (tokens[0] == "map_Pr"){
            current->map_roughness = new Texture(texture_path + tokens[1]);
        }
        else if (tokens[0] == "map_Pm"){
            current->map_metallic = new Texture(texture_path + tokens[1]);
        }
        else if (tokens[0] == "map_Bump"){
            current->map_normal = new Texture(texture_path + tokens[3]);
        }
    }
    file.close();

    return output;
}

ShaderStore::ShaderStore(){}

ShaderStore::ShaderStore(std::map<std::string, Shader*> shaders){
    this->shaders = shaders;
}

Shader* ShaderStore::Get(const std::string &name){
    return shaders[name];
}

void ShaderStore::LoadShaders(const std::string &filename){
    shaders.merge(ParseMTL(filename));
}

void ShaderStore::Add(Shader *mat, const std::string &name){
    shaders[name] = mat;
}

void ShaderStore::SetSceneInfo(mat4 model, mat4 view, mat4 proj, vec3 cam_pos, SceneLighting *light_info){
    num_lights = light_info->num_lights;
    for (auto const &[name, shader] : shaders){
        shader->model = model;
        shader->view = view;
        shader->proj = proj;
        shader->cam_pos = cam_pos;
        shader->light_info = light_info;
    }
}

std::ostream& operator<<(std::ostream &os, const ShaderStore &m){
    for (auto const& [name, shader] : m.shaders){
        os << name << '\n';
    }
    return os;
}