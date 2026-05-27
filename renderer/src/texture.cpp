#include "renderer/texture.h"

Texture::Texture(vec3 **p_image, int width, int height){
    image = p_image;
    w = width;
    h = height;
}
Texture::Texture(const std::string &path){
    int width;
    int height;

    image = loadimage(path, width, height);
    w = width;
    h = height;
}
Texture::Texture(){
    image = new vec3*[0];
    w = 0;
    h = 0;
}

vec3 Texture::sample(vec2 uv){
    if (w == 0 || h == 0) return vec3(1, 0, 1);

    vec2 repeat_uv = vec2(uv.x - floor(uv.x), uv.y - floor(uv.y));
    return image[int(repeat_uv.x*float(w-1))][int(repeat_uv.y*float(h-1))];
}

vec3 **Texture::loadimage(const std::string &path, int &width, int &height){
    int channels;
    unsigned char *img = stbi_load(path.c_str(), &width, &height, &channels, 3);

    if (img == NULL){
        printf("Texture: failed to load image");
        exit(1);
    }

    vec3 **result = new vec3*[width];
    for (int i = 0; i < width; i++){
        result[i] = new vec3[height];
    }

    int img_index = 0;
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            result[j][i] = vec3(img[img_index], img[img_index+1], img[img_index+2])/255.0f;
            img_index += 3;
        }
    }

    return result;
}