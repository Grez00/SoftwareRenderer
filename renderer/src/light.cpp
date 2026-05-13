#include "renderer/light.h"

vec3 dirlight::GetDirection(){
    return direction;
}

vec3 pointlight::GetDirection(vec3 p){
    return normalize(p - position);
}