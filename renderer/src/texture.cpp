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

Texture *CubeMap::operator[](int i){
    switch (i){
        case 0:
            return right;
        case 1:
            return left;
        case 2:
            return top;
        case 3:
            return bottom;
        case 4:
            return back;
        case 5:
            return front;
        default:
            printf("CubeMap: Index out of bounds error\n");
            exit(-1);
    }
}

CubeMap::CubeMap(){
    left, right, top, bottom, front, back = new Texture();
}
CubeMap::CubeMap(std::vector<std::string> paths){
    if (paths.size() != 6){
        printf("Cubemap: Incorrect number of faces provided\n");
        exit(-1);
    }

    right = new Texture(paths[0]);
    left = new Texture(paths[1]);
    top = new Texture(paths[2]);
    bottom = new Texture(paths[3]);
    back = new Texture(paths[4]);
    front = new Texture(paths[5]);
}
CubeMap::CubeMap(
    const std::string &right_path, 
    const std::string &left_path, 
    const std::string &top_path,
    const std::string &bottom_path,
    const std::string &back_path,
    const std::string &front_path
){
    left = new Texture(left_path);
    right = new Texture(right_path);
    top = new Texture(top_path);
    bottom = new Texture(bottom_path);
    front = new Texture(front_path);
    back = new Texture(back_path);
}

vec3 CubeMap::sample(vec3 dir){
    vec3 abs_dir = abs(dir);

    bool is_x = abs_dir.x > abs_dir.y && abs_dir.x > abs_dir.z;
    bool is_y = !is_x && abs_dir.y > abs_dir.z;

    float ma, sc, tc;
    int face_index;

    if (is_x){
        ma = abs_dir.x;
        tc = -dir.y;

        if (dir.x > 0){
            face_index = 0;
            sc = -dir.z;
        }
        else{
            face_index = 1;
            sc = dir.z;
        }
    }
    else if (is_y){
        ma = abs_dir.y;
        sc = dir.x;
        
        if (dir.y > 0){
            face_index = 2;
            tc = dir.z;
        }
        else{
            face_index = 3;
            tc = -dir.z;
        }
    }
    else{
        ma = abs_dir.z;
        tc = -dir.y;
        
        if (dir.z > 0){
            face_index = 4;
            sc = dir.x;
        }
        else{
            face_index = 5;
            sc = -dir.x;
        }
    }

    vec2 uv = vec2(
        0.5f * (sc / ma + 1),
        0.5f * (tc / ma + 1)
    );

    vec3 col;
    switch (face_index){
        case 0:
            col = vec3(0, 0, 1);
            break;
        case 1:
            col = vec3(0, 1, 0);
            break;
        case 2:
            col = vec3(1, 0, 0);
            break;
        case 3:
            col = vec3(0, 1, 1);
            break;
        case 4:
            col = vec3(1, 1, 0);
            break;
        case 5:
            col = vec3(1, 0, 1);
            break;
        default:
            printf("CubeMap: Index out of bounds error\n");
            exit(-1);
    }

    return (*this)[face_index]->sample(uv);
    //return col;
}