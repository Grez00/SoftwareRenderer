#ifndef SCENE_H
#define SCENE_H

#include "camera.h"
#include "mesh.h"
#include "light.h"
#include "framebuffer.h"
#include "vectors.h"
#include "mat4.h"
#include "transformation.h"
#include "drawing.h"

#include <map>
#include <string>
#include <any>
#include <vector>

int PrintNestedVector(std::vector<std::any> vector);
int PrintNestedMap(std::map<std::string, std::any> map);

class Scene{
    public:
        Camera main_cam;
        SceneLighting lighting_info;

        int num_models;
        Model *models;

        int num_textures;
        Texture *textures;

        int num_mats;
        Shader *shaders;

        Scene(const std::string &filename);

        void Draw(FrameBuffer *buffer);
};

#endif