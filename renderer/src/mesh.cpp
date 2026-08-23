#include "renderer/mesh.h"

Mesh::Mesh(vec4 *p_positions, vec3 *p_normals, vec2 *p_uvs, vec3 *p_indices, int p_vertcount, int p_indexcount){
    positions = p_positions;
    normals = p_normals;
    uvs = p_uvs;
    indices = p_indices;
    vert_count = p_vertcount;
    index_count = p_indexcount;
}

void CalculateTangentSpace(vec4 *tangents, Mesh *mesh){
    vec3 *tan = new vec3[mesh->vert_count*2];
    vec3 *bi_tan = tan + mesh->vert_count;

    for (int i = 0; i < mesh->index_count; i+=3){

        vec3 i0 = mesh->indices[i];
        vec3 i1 = mesh->indices[i+1];
        vec3 i2 = mesh->indices[i+2];

        vec3 p0 = mesh->positions[int(i0.x)];
        vec3 p1 = mesh->positions[int(i1.x)];
        vec3 p2 = mesh->positions[int(i2.x)];

        vec2 uv0 = mesh->uvs[int(i0.y)];
        vec2 uv1 = mesh->uvs[int(i1.y)];
        vec2 uv2 = mesh->uvs[int(i2.y)];

        vec3 edge_0 = p1 - p0;
        vec3 edge_1 = p2 - p0;

        vec2 delta_uv_0 = uv1 - uv0;
        vec2 delta_uv_1 = uv2 - uv0;

        float f = 1.0f / (delta_uv_0.x * delta_uv_1.y - delta_uv_1.x * delta_uv_0.x);

        vec3 tangent = vec3(
            f * (delta_uv_1.y * edge_0.x - delta_uv_0.y * edge_1.x),
            f * (delta_uv_1.y * edge_0.y - delta_uv_0.y * edge_1.y),
            f * (delta_uv_1.y * edge_0.z - delta_uv_0.y * edge_1.z)
        );

        vec3 bi_tangent = vec3(
            f * (-delta_uv_1.x * edge_0.x + delta_uv_0.x * edge_1.x),
            f * (-delta_uv_1.x * edge_0.y + delta_uv_0.x * edge_1.y),
            f * (-delta_uv_1.x * edge_0.z + delta_uv_0.x * edge_1.z)
        );

        tan[int(i0.x)] += tangent;
        tan[int(i1.x)] += tangent;
        tan[int(i2.x)] += tangent;

        bi_tan[int(i0.x)] += bi_tangent;
        bi_tan[int(i1.x)] += bi_tangent;
        bi_tan[int(i2.x)] += bi_tangent;
    }

    tangents = new vec4[mesh->vert_count];

    for (int i = 0; i < mesh->index_count; i++){
        vec3 index = mesh->indices[i];

        vec3 tangent = tan[int(index.x)];
        vec3 bi_tangent = bi_tan[int(index.x)];
        vec3 normal = mesh->normals[int(index.z)];

        tangents[int(index.x)] = v3tov4(
            normalize(tangent - normal * dot(normal, tangent)),
            (dot(cross(normal, tangent), bi_tangent) < 0.0f) ? -1.0f : 1.0f 
        );
    }
}

Mesh::Mesh(const std::string &filename){
    std::string file_end = ".obj";
    if (filename.length() < file_end.length() || filename.compare(filename.length() - file_end.length(), file_end.length(), file_end)){
        printf("LoadMesh: Error, invalid file type (got: %s)\n", filename.c_str());
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
    file.close();

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

    positions = p_positions;
    normals = p_normals;
    uvs = p_uvs;
    indices = p_indices;

    vert_count = seen_pos.size();
    index_count = seen_indices.size();
    triangle_count = index_count/3;

    CalculateTangentSpace(tangents, this);
}

Mesh::Mesh(){
    positions = new vec4[0];
    normals = new vec3[0];
    uvs = new vec2[0];
    indices = new vec3[0];

    vert_count = 0;
    index_count = 0;
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

Model::Model(const std::string &filename){
    std::string file_end = ".obj";
    if (filename.length() < file_end.length() || filename.compare(filename.length() - file_end.length(), file_end.length(), file_end)){
        printf("LoadMesh: Error, invalid file type (got: %s)\n", filename.c_str());
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

    shaders = new ShaderStore();
    index_to_mat = std::map<int, std::string>();

    // Search for .mtl file

    while (getline(file, line)){
        std::vector<std::string> tokens = split(line, " ");

        if (tokens[0] == "mtllib"){
            shaders->LoadShaders("assets/materials/" + tokens[1]);
            break;
        }
        else if (tokens[0] == "o"){
            break;
        }
    }

    // Load mesh

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
        else if (tokens[0] == "usemtl"){
            index_to_mat[seen_indices.size()] = tokens[1];
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
    file.close();

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

    mesh = new Mesh(p_positions, p_normals, p_uvs, p_indices, seen_pos.size(), seen_indices.size());

    // Calculate Tangent Space
    vec4 *tangents;
    CalculateTangentSpace(tangents, mesh);
    mesh->tangents = tangents;
}

Model::Model() {}
Model::Model(Mesh *mesh){
    this->mesh = mesh;
}
Model::Model(Mesh *mesh, ShaderStore *shaders, mat4 model){
    this->mesh = mesh;
    this->shaders = shaders;
    this->matrix = model;
}

void Model::LinkMaterials(ShaderStore *shaders){
    this->shaders = shaders;
}
void Model::LinkMaterial(Shader *mat, const std::string &name){
    shaders->Add(mat, name);
}

void Model::LinkMatrix(mat4 model){
    this->matrix = model;
}