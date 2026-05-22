#include "renderer/camera.h"

Camera::Camera(vec3 pPosition, vec3 pUp, vec3 pForward){
    position = pPosition;
    world_up = pUp;
    forward = pForward;
    UpdateVectors();
}

mat4 Camera::GetViewMatrix(){
    mat4 m1 = mat4(
        right.x, right.y, right.z, 0.0f,
        up.x, up.y, up.z, 0.0f,
        direction.x, direction.y, direction.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    mat4 m2 = mat4(
        1.0f, 0.0f, 0.0f, -position.x,
        0.0f, 1.0f, 0.0f, -position.y,
        0.0f, 0.0f, 1.0f, -position.z,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    return m1*m2;
}

void Camera::UpdateVectors(){
    direction = normalize(-forward);

    right = normalize(cross(world_up, direction));
    up = normalize(cross(direction, right));
}