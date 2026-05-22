#ifndef TEXTURE_H
#define TEXTURE_H

#include <math.h>

#include "stb_image.h"
#include "vectors.h"

class Texture{
    public:
        int w;
        int h;
        vec3 **image;

        Texture(vec3 **p_image, int width, int height);
        Texture(const std::string &path);
        Texture();

        vec3 sample(vec2 uv);
    
    private:
        vec3 **loadimage(const std::string &path, int &width, int &height);
};

#endif