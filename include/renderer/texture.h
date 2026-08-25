#ifndef TEXTURE_H
#define TEXTURE_H

#include <math.h>
#include <vector>

#include "stb_image.h"
#include "vectors.h"

enum SAMPLE_TYPE {
    POINT,
    BILINEAR
};

class Texture{
    public:
        int w;
        int h;
        vec3 **image;

        Texture(vec3 **p_image, int width, int height);
        Texture(const std::string &path);
        Texture(const std::string &path, int num_mipmaps);
        Texture();

        vec3 sample(vec2 uv);
        vec3 sample(vec2 uv, SAMPLE_TYPE type);

        bool IsEmpty();
    
    private:
        vec3 **loadimage(const std::string &path, int &width, int &height);
};

vec3 **DownSample(vec3 **image, int w, int h, int factor);

class CubeMap{
    public:
        Texture *left, *right, *top, *bottom, *front, *back;

        CubeMap();
        CubeMap(std::vector<std::string> paths);
        CubeMap(
            const std::string &left_path, 
            const std::string &right_path, 
            const std::string &top_path,
            const std::string &bottom_path,
            const std::string &front_path,
            const std::string &back_path
        );

        Texture *operator[](int i);

        vec3 sample(vec3 dir);
        bool IsInvalid();
};

#endif