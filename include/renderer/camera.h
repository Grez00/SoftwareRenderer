#ifndef CAMERA_H
#define CAMERA_H

#include "vectors.h"
#include "mat4.h"
#include "rendermats.h"

class Camera{
    public:
        vec3 position;
        vec3 forward;
        vec3 up;
        vec3 right = vec3(1.0f, 0.0f, 0.0f);

        float near;
        float far;

        float fov;
        float aspect;

        mat4 view;
        mat4 proj;

        Camera(
            vec3 pPosition = vec3(0.0f, 0.0f, 0.0f), 
            vec3 pUp = vec3(0.0f, 1.0f, 0.0f), 
            vec3 pForward = vec3(0.0f, 0.0f, -1.0f),
            float p_fov = 45.0f, 
            float p_aspect = 4.0f/3.0f, 
            float n = 0.01f, 
            float f = 100.0f
        );
        void UpdateVectors();

    private:
        vec3 world_up = vec3(0.0f, 1.0f, 0.0f);
        vec3 direction = vec3(0.0f, 0.0f, -1.0f);
};

#endif