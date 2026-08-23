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
Texture::Texture(const std::string &path, int p_num_mipmaps){
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

vec3 Texture::sample(vec2 uv, SAMPLE_TYPE type){
    if (w == 0 || h == 0) return vec3(1, 0, 1);
    vec2 repeat_uv = vec2(uv.x - floor(uv.x), uv.y - floor(uv.y));

    if (type == POINT){
        return image[int(repeat_uv.x*float(w-1))][int(repeat_uv.y*float(h-1))];
    }
    else if (type == BILINEAR){
        float u = uv.x * float(w-1);
        float v = uv.y * float(h-1);

        int low_u = floor(u);
        int high_u = ceil(u);

        int low_v = floor(v);
        int high_v = ceil(v);

        vec3 s_11 = image[low_u][low_v];
        vec3 s_12 = image[low_u][high_v];
        vec3 s_21 = image[high_u][low_v];
        vec3 s_22 = image[high_u][high_v];

        float t_x = u - trunc(u);
        float t_y = v - trunc(v);

        return lerp(lerp(s_11, s_21, t_x), lerp(s_12, s_22, t_x), t_y);
    }
    else{
        return vec3();
    }
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

bool Texture::IsEmpty(){
    return w == 0 || h == 0;
}

vec3 **DownSample(vec3 **image, int w, int h, int factor){
    int d_w = int(float(w)/float(factor));
    int d_h = int(float(h)/float(factor));

    vec3 **result = new vec3*[d_w];
    for (int i = 0; i < d_w; i++){
        result[i] = new vec3[d_h];
    }

    for (int x = 0; x < d_w; x++){
        for (int y = 0; y < d_h; y++){
            vec3 average = vec3(1, 1, 1);
            for (int i = 0; i < factor; i++){
                for (int j = 0; j < factor; j++){
                    average *= image[x*factor + i][y*factor + j];
                }
            }
            result[x][y] = average;
        }
    }

    return result;
}