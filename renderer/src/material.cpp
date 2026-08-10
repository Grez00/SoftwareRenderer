#include "renderer/material.h"

Material::Material() {}
Material::Material(Shader *shader){
    this->shader = shader;
}

void Material::Reconstruct(){
    StandardVertex vert;
    BlinnPhongShader frag;
    Shader shader = Shader(&vert, &frag);
    this->shader = &shader;

    shader.params.SetFloat("shininess", this->shininess);
    shader.params.SetVector3("ambient", this->ambient);
    shader.params.SetVector3("diffuse", this->diffuse);
    shader.params.SetVector3("specular", this->specular);
    shader.params.SetTexture("tex", this->tex);
}

std::map<std::string, Material> ParseMTL(const std::string &filename){
    std::map<std::string, Material> output;

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

    std::string current;
    while(getline(file, line)){
        std::vector<std::string> tokens = split(line, " ");

        if (tokens[0] == "newmtl"){
            current = tokens[1];
            output[tokens[1]] = Material();
        }
        else if (tokens[0] == "Ns"){
            output[current].shininess = std::stof(tokens[1]);
        }
        else if (tokens[0] == "Ka"){
            output[current].ambient = vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
        }
        else if (tokens[0] == "Kd"){
            output[current].diffuse = vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));;
        }
        else if (tokens[0] == "Ks"){
            output[current].specular = vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));;
        }
        else if (tokens[0] == "map_Kd"){
            output[current].tex = new Texture(tokens[1]);
        }
    }
    file.close();

    for (auto &[name, material] : output){
        material.Reconstruct();
    }
    return output;
}

MaterialStore::MaterialStore(){}

Material MaterialStore::Get(const std::string &name){
    return materials[name];
}

void MaterialStore::LoadMaterials(const std::string &filename){
    materials.merge(ParseMTL(filename));
}

void MaterialStore::Add(Material *mat, const std::string &name){
    materials[name] = *mat;
}

std::ostream& operator<<(std::ostream &os, const MaterialStore &m){
    for (auto const& [name, material] : m.materials){
        os << name << ":" << '\n';
        os << '\t' << "ambient: " << material.ambient << '\n';
        os << '\t' << "diffuse: " << material.diffuse << '\n';
        os << '\t' << "specular: " << material.specular << '\n';
        os << '\t' << "shininess: " << material.shininess << '\n';
    }
    return os;
}