#ifndef CAMERA_H
#define CAMERA_H

#include "vec2.h"
#include "vec3.h"
#include "mat4.h"
#include "rendermats.h"

class Camera{
    public:
        vec3 position;
        vec3 forward;
        vec3 up;
        vec3 right = vec3(1.0f, 0.0f, 0.0f);

        Camera(vec3 pPosition = vec3(0.0f, 0.0f, 0.0f), vec3 pUp = vec3(0.0f, 1.0f, 0.0f), vec3 pForward = vec3(0.0f, 0.0f, -1.0f));
        mat4 GetViewMatrix();
        void UpdateVectors();

    private:
        vec3 world_up = vec3(0.0f, 1.0f, 0.0f);
};

#endif