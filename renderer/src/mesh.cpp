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

    if (!file.is_open()){
        printf("LoadMesh: Error, failed to open file, filename: %s\n", filename.c_str());
        return;
    }

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
                switch (face_tokens.size()){
                    case 3:
                        seen_indices.push_back(vec3(std::stof(face_tokens[0])-1, std::stof(face_tokens[1])-1, std::stof(face_tokens[2])-1));
                        break;
                    case 2:
                        seen_indices.push_back(vec3(std::stof(face_tokens[0])-1, std::stof(face_tokens[1])-1, 0));
                        break;
                    case 1:
                        seen_indices.push_back(vec3(std::stof(face_tokens[0])-1, 0, 0));
                        break;
                    default:
                        seen_indices.push_back(vec3());
                        break;
                }
            }
        }
    }

    p_positions = new vec4[seen_pos.size()];
    std::copy(seen_pos.begin(), seen_pos.end(), p_positions);

    p_indices = new vec3[seen_indices.size()];
    std::copy(seen_indices.begin(), seen_indices.end(), p_indices);

    if (seen_normals.size() == 0){
        p_normals = new vec3[1];
        p_normals[0] = vec3();
    }
    else{
        p_normals = new vec3[seen_normals.size()];
        std::copy(seen_normals.begin(), seen_normals.end(), p_normals);
    }

    if (seen_uvs.size() == 0){
        p_uvs = new vec2[1];
        p_uvs[0] = vec2();
    }
    else{
        p_uvs = new vec2[seen_uvs.size()];
        std::copy(seen_uvs.begin(), seen_uvs.end(), p_uvs);
    }

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

sphere Mesh::GetBoundingSphere(){
    vec3 min = 9999999.9f;
    vec3 max = -9999999.9f;

    for (int i = 0; i < vert_count; i++){
        if (positions[i].x < min.x) min.x = positions[i].x;
        if (positions[i].y < min.y) min.y = positions[i].y;
        if (positions[i].z < min.z) min.z = positions[i].z;

        if (positions[i].x > max.x) max.x = positions[i].x;
        if (positions[i].y > max.y) max.y = positions[i].y;
        if (positions[i].z > max.z) max.z = positions[i].z;
    }
    vec3 diff = max - min;
    vec3 center = (max+min)*0.5f;
    float radius = std::max(diff.x, std::max(diff.y, diff.z)) / 2.0f;

    float sq_radius = radius*radius;
    for (int i = 0; i < vert_count; i++){
        vec3 dir = positions[i] - center;
        float dist = length(dir);
        float sq_dist = dist*dist;

        if (sq_dist > sq_radius){
            float difference = (dist - radius)/2.0f;
            radius += difference;
            sq_radius = radius*radius;
            center += difference * dir;
        }
    }

    return sphere(center, radius);
}