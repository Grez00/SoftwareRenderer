#ifndef LIGHT_H
#define LIGHT_H

#include "vec3.h"

typedef struct light{
    vec3 colour;
    float strength;
    virtual vec3 GetDirection();
} light;

typedef struct dirlight : light{
    vec3 direction;
    vec3 GetDirection() override;
} dirlight;

typedef struct pointlight : light{
    vec3 position;
    vec3 GetDirection(vec3 p);
} pointlight;

#endif