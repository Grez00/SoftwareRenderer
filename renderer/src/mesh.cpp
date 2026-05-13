#include "renderer/mesh.h"

Mesh::Mesh(vec4 *p_positions, vec3 *p_normals, vec2 *p_uvs, vec3 *p_indices, int p_vertcount, int p_indexcount){
    positions = p_positions;
    normals = p_normals;
    uvs = p_uvs;
    indices = p_indices;
    vert_count = p_vertcount;
    index_count = p_indexcount;
}

Mesh::Mesh(const std::string &filename){
    std::string file_end = ".obj";
    if (filename.length() < file_end.length() || filename.compare(filename.length() - file_end.length(), file_end.length(), file_end)){
        printf("LoadMesh: Error, invalid file name (got: %s)\n", filename.c_str());
        return;
    }

    std::string line;
    std::ifstream file(filename);

    vec4 *p_positions;
    vec3 *p_normals;
    vec2 *p_uvs;
    vec3 *p_indices;

    std::vector<vec4> seen_pos = {};
    std::vector<vec3> seen_normals = {};
    std::vector<vec2> seen_uvs = {};
    std::vector<vec3> seen_indices = {};

    while(getline(file, line)){
        std::vector<std::string> tokens = split(line, " ");

        if (tokens[0] == "v"){
            seen_pos.push_back(vec4(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
        }
        else if (tokens[0] == "vn"){
            seen_normals.push_back(vec3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3])));
        }
        else if (tokens[0] == "vt"){
            seen_uvs.push_back(vec2(std::stof(tokens[1]), std::stof(tokens[2])));
        }
        else if (tokens[0] == "f"){
            std::vector<std::string> face_tokens;
            for (int i = 1; i < 4; i++){
                face_tokens = split(tokens[i], "/");
                seen_indices.push_back(vec3(std::stof(face_tokens[0])-1, std::stof(face_tokens[1])-1, std::stof(face_tokens[2])-1));
            }
        }
    }

    p_positions = new vec4[seen_pos.size()];
    p_normals = new vec3[seen_normals.size()];
    p_uvs = new vec2[seen_uvs.size()];
    p_indices = new vec3[seen_indices.size()];

    std::copy(seen_pos.begin(), seen_pos.end(), p_positions);
    std::copy(seen_normals.begin(), seen_normals.end(), p_normals);
    std::copy(seen_uvs.begin(), seen_uvs.end(), p_uvs);
    std::copy(seen_indices.begin(), seen_indices.end(), p_indices);

    file.close();

    positions = p_positions;
    normals = p_normals;
    uvs = p_uvs;
    indices = p_indices;

    vert_count = seen_pos.size();
    index_count = seen_indices.size();
}

Mesh::Mesh(){
    positions = new vec4[0];
    normals = new vec3[0];
    uvs = new vec2[0];
    indices = new vec3[0];

    vert_count = 0;
    index_count = 0;
}

void Mesh::LinkTexture(Texture p_tex){
    tex = p_tex;
}