#include "renderer/shader.h"

Shader::Shader() {
    world_pos = new vec3[3];
}

vec3 Shader::EvaluateFragment(vertex2D v){
    return vec3(1, 0, 1);
}

vertex Shader::EvaluateVertex(vertex v){
    mat4 M_V = view * model;
    mat3 id_M_V = mat3(transpose(inverse(M_V)));
    return vertex(proj * M_V * v.position, id_M_V * v.normal, v.uv);
}

Triangle3D Shader::EvaluateTriangle(Triangle3D tri){
    world_pos[0] = vec3(model * tri.vertices[0].position);
    world_pos[1] = vec3(model * tri.vertices[1].position);
    world_pos[2] = vec3(model * tri.vertices[2].position);

    mat4 M_V = view * model;
    mat4 M_V_P = proj * M_V;
    mat3 id_M = mat3(transpose(inverse(model)));

    return Triangle3D(
        vertex(M_V_P * tri.vertices[0].position, id_M * tri.vertices[0].normal, tri.vertices[0].uv),
        vertex(M_V_P * tri.vertices[1].position, id_M * tri.vertices[1].normal, tri.vertices[1].uv),
        vertex(M_V_P * tri.vertices[2].position, id_M * tri.vertices[2].normal, tri.vertices[2].uv)
    );
}

NormalShader::NormalShader() {}
vec3 NormalShader::EvaluateFragment(vertex2D v){
    return v.normal;
}

MaterialShader::MaterialShader() {}
MaterialShader::MaterialShader(vec3 ambient, vec3 diffuse, vec3 specular, float shininess){
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;
}
vec3 MaterialShader::EvaluateFragment(vertex2D v){
    return vec3(1, 1, 1) * ambient * diffuse * specular;
}

BlinnPhongShader::BlinnPhongShader() {}
BlinnPhongShader::BlinnPhongShader(Texture *tex){
    this->tex = tex;
}
BlinnPhongShader::BlinnPhongShader(vec3 ambient, vec3 diffuse, vec3 specular, float shininess, Texture *tex){
    this->ambient = ambient;
    this->diffuse = diffuse;
    this->specular = specular;
    this->shininess = shininess;
    this->tex = tex;
}
vec3 BlinnPhongShader::EvaluateFragment(vertex2D v){
    vec3 tex_col = vec3(1, 1, 1);
    if (tex != NULL) tex_col = tex->sample(v.uv);

    vec3 view_dir = normalize(cam_pos - v.position);

    vec3 col = vec3();
    for (int i = 0; i < light_info->num_dir_lights; i++){
        col += light_info->dir_lights[i].Evaluate(v.normal, view_dir, ambient, diffuse, specular, shininess);
    }
    for (int i = 0; i < light_info->num_p_lights; i++){
        col += light_info->p_lights[i].Evaluate(v.normal, view_dir, v.position, ambient, diffuse, specular, shininess);
    }

    return tex_col * col;
}

TextureShader::TextureShader() {}
TextureShader::TextureShader(Texture *tex){
    this->tex = tex;
}
vec3 TextureShader::EvaluateFragment(vertex2D v){
    return tex->sample(v.uv);
}

ColorShader::ColorShader() {
    col = vec3(1, 0, 1);
}
ColorShader::ColorShader(vec3 col){
    this->col = col;
}
vec3 ColorShader::EvaluateFragment(vertex2D v){
    return col;
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

    BlinnPhongShader *current;
    while(getline(file, line)){
        std::vector<std::string> tokens = split(line, " ");

        if (tokens[0] == "newmtl"){
            current = new BlinnPhongShader();
            current->tex = new Texture();
            output[tokens[1]] = current;
        }
        else if (tokens[0] == "Ns"){
            current->shininess = std::stof(tokens[1]);
        }
        else if (tokens[0] == "Ka"){
            current->ambient = vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        }
        else if (tokens[0] == "Kd"){
            current->diffuse = vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));;
        }
        else if (tokens[0] == "Ks"){
            current->specular = vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));;
        }
        else if (tokens[0] == "map_Kd"){
            current->tex = new Texture(tokens[1]);
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