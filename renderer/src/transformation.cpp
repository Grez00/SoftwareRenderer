#include "renderer/transformation.h"

// Transformation

mat4 GetTranslationMatrix(vec3 offset){
    float offset_cells[4][4] = {
        {1.0f, 0.0f, 0.0f, offset.x},
        {0.0f, 1.0f, 0.0f, offset.y},
        {0.0f, 0.0f, 1.0f, offset.z},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return mat4(offset_cells);
}

mat4 GetScalingMatrix(vec3 scale){
    float scale_cells[4][4] = {
        {scale.x, 0.0f, 0.0f, 0.0f},
        {0.0f, scale.y, 0.0f, 0.0f},
        {0.0f, 0.0f, scale.z, 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return mat4(scale_cells);
}

mat4 GetRotationMatrix(float rotation, vec3 axis, vec3 r_point){
    axis = normalize(axis);

    mat4 t = GetTranslationMatrix(r_point * -1.0f);
    mat4 i_t = GetTranslationMatrix(r_point);
    
    float rotation_cells[4][4] = {
        {cos(rotation) + axis.x*axis.x * (1 - cos(rotation)), axis.x * axis.y * (1 - cos(rotation)) - axis.z * sin(rotation), axis.x * axis.z * (1 - cos(rotation)) + axis.y * sin(rotation), 0.0f},
        {axis.y * axis.x * (1 - cos(rotation)) + axis.z * sin(rotation), cos(rotation) + axis.y*axis.y * (1 - cos(rotation)), axis.y * axis.z * (1 - cos(rotation)) - axis.x * sin(rotation), 0.0f},
        {axis.z * axis.x * (1 - cos(rotation)) - axis.y * sin(rotation), axis.z * axis.y * (1 - cos(rotation)) + axis.x * sin(rotation), cos(rotation) + axis.z*axis.z * (1 - cos(rotation)), 0.0f},
        {0.0f, 0.0f, 0.0f, 1.0f}
    };
    return i_t * mat4(rotation_cells) * t;
}

mat4 GetModelMatrix(vec3 offset, vec3 scale, float rotation, vec3 axis, vec3 r_point){
    mat4 offset_matrix = GetTranslationMatrix(offset);
    mat4 rotation_matrix = GetRotationMatrix(rotation, axis, r_point);
    mat4 scale_matrix = GetScalingMatrix(scale);

    return offset_matrix * rotation_matrix * scale_matrix;
}