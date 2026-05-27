#include "renderer/camera.h"

Camera::Camera(vec3 pPosition, vec3 pUp, vec3 pForward, float p_fov, float p_aspect, float n, float f){
    position = pPosition;
    world_up = pUp;
    forward = pForward;

    near = n;
    far = f;

    fov = p_fov;
    aspect = p_aspect;

    proj = GetProjectionMatrix(fov, aspect, near, far);

    UpdateVectors();
}

void Camera::UpdateVectors(){
    direction = normalize(-forward);

    right = normalize(cross(world_up, direction));
    up = normalize(cross(direction, right));

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

    view = m1*m2;
}