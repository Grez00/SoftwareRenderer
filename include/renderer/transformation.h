#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "mat4.h"
#include "vectors.h"
#include <math.h>

mat4 GetTranslationMatrix(vec3 offset = vec3(0.0f, 0.0f, 0.0f));
mat4 GetScalingMatrix(vec3 scale = vec3(1.0f, 1.0f, 1.0f));
mat4 GetRotationMatrix(float rotation = 0.0f, vec3 axis = vec3(1.0f, 0.0f, 0.0f), vec3 r_point = vec3(0.0f, 0.0f, 0.0f));
mat4 GetModelMatrix(vec3 offset = vec3(0.0f, 0.0f, 0.0f), vec3 scale = vec3(1.0f, 1.0f, 1.0f), float rotation = 0.0f, vec3 axis = vec3(1.0f, 0.0f, 0.0f), vec3 r_point = vec3(0.0f, 0.0f, 0.0f));

#endif