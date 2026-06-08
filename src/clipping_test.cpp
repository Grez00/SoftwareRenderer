#include <iostream>
#include <math.h>
#include <sys/time.h>

#include <renderer/renderer.h>
#include <SDL3/SDL.h>

const int SCR_WIDTH = 800/2, SCR_HEIGHT = 600/2; // Pixels to render
const int RENDER_SCALE = 2; // Multiplier for the actual scale of the window

const float MOVE_SPEED = 50.0f;
const float ROTATION_SPEED = 25.0f;

const bool frustum_cull = false;

// Get the time of day in seconds
double GetCurrentTime(){
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return ((double) tv.tv_sec) + ((double) tv.tv_usec / 1000000.0);
}

// Output the contents of a RenderBuffer to an SDL_Renderer
void BlitBuffer(FrameBuffer buffer, SDL_Texture *sdl_buffer, SDL_Renderer *renderer){
    void* pixels;
    int pitch = 0;

    SDL_LockTexture(sdl_buffer, NULL, &pixels, &pitch);
    memcpy(pixels, buffer.render_buffer, buffer.h * pitch);
    SDL_UnlockTexture(sdl_buffer);
    SDL_RenderTexture(renderer, sdl_buffer, NULL, NULL);
}

void BlitTexture(Texture tex, SDL_Renderer *renderer){
    for (int i = 0; i < SCR_HEIGHT; i++){
        for (int j = 0; j < SCR_WIDTH; j++){
            vec3 sampled_col = tex.sample(vec2(float(j)/float(SCR_WIDTH), float(i)/float(SCR_HEIGHT)));
            SDL_SetRenderDrawColor(
                renderer, 
                sampled_col.x * 255.0f, 
                sampled_col.y * 255.0f, 
                sampled_col.z * 255.0f,
                SDL_ALPHA_OPAQUE
            );
            SDL_RenderPoint(renderer, j, i);
        }
    }
}

int HandleInput(vec3 &offset, int &cam_to_use, Camera *cam, float delta_time, SDL_Event event){
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT){
            return -1;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN){
            float velocity = MOVE_SPEED * delta_time;
            float rotation_velocity = ROTATION_SPEED * delta_time;
            if (event.key.key == SDLK_W) {
                cam->position += cam->forward * velocity;
            } else if (event.key.key == SDLK_A) {
                cam->position -= cam->right * velocity;
            } else if (event.key.key == SDLK_S) {
                cam->position -= cam->forward * velocity;
            } else if (event.key.key == SDLK_D) {
                cam->position += cam->right * velocity;
            } else if (event.key.key == SDLK_Q) {
                cam->forward = normalize(GetRotationMatrix(rotation_velocity, cam->up) * cam->forward);
            } else if (event.key.key == SDLK_E) {
                cam->forward = normalize(GetRotationMatrix(-rotation_velocity, cam->up) * cam->forward);
            } else if (event.key.key == SDLK_R) {
                cam->forward = vec3(0, 0, 1);
            } else if (event.key.key == SDLK_T) {
                cam->forward = vec3(-1, 0, 0);
            } else if (event.key.key == SDLK_LEFT){
                cam_to_use = 0;
                //offset += vec3(-velocity, 0, 0);
            } else if (event.key.key == SDLK_RIGHT){
                cam_to_use = 1;
                //offset += vec3(velocity, 0, 0);
            } else if (event.key.key == SDLK_UP){
                offset += vec3(0, 0, velocity);
            } else if (event.key.key == SDLK_DOWN){
                offset += vec3(0, 0, -velocity);
            }

            cam->UpdateVectors();
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    SDL_Init(SDL_INIT_VIDEO);

    // Create Window
    SDL_Window *window = SDL_CreateWindow("SDL3", SCR_WIDTH*RENDER_SCALE, SCR_HEIGHT*RENDER_SCALE, SDL_WINDOW_HIGH_PIXEL_DENSITY);
    if (window == NULL){
        std::cout << "Window creation failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    // Create Renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == NULL){
        std::cout << "Renderer creation failed: " << SDL_GetError() << std::endl;
        return -1;
    }
    SDL_SetRenderScale(renderer, RENDER_SCALE, RENDER_SCALE);

    // Create output texture
    SDL_Texture *sdl_buffer = SDL_CreateTexture
    (
        renderer,
        SDL_PIXELFORMAT_RGB24,
        SDL_TEXTUREACCESS_STREAMING, 
        SCR_WIDTH,
        SCR_HEIGHT
    );
    SDL_SetTextureScaleMode(sdl_buffer, SDL_SCALEMODE_NEAREST);

    // Create Frame Buffer
    FrameBuffer render_buffer = FrameBuffer(SCR_WIDTH, SCR_HEIGHT);

    // Set up geometry
    vec4 v0 = vec4(-0.5f, -0.5f, 0.0f);
    vec4 v1 = vec4(0.5f, -0.5f, 0.0f);
    vec4 v2 = vec4(0.0f, 0.5f, 0.0f);

    // Create Camera
    Camera main_cam = Camera(vec3(0, 24, 0), vec3(0, 0, -1), vec3(0, -1, 0));
    Camera secondary_cam = Camera(vec3(), vec3(0, 1, 0), vec3(0, 0, -1));

    // Create matrices
    mat4 view = main_cam.view;
    mat4 proj = main_cam.proj;

    // Load models
    Mesh cube = Mesh("assets/models/cube.obj");
    Mesh bishop = Mesh("assets/models/bishop.obj");
    Mesh icosphere = Mesh("assets/models/icosphere.obj");
    Mesh sphere_mesh = Mesh("assets/models/sphere.obj");

    // Load Textures
    Texture tex = Texture("assets/images/worldsky.png");
    Texture magic = Texture("assets/images/magic.jpg");

    // Link Textures
    cube.LinkTexture(tex);
    icosphere.LinkTexture(tex);
    bishop.LinkTexture(tex);
    sphere_mesh.LinkTexture(tex);

    aabb test_box = aabb(vec3(0, 0, -7), vec3(2, 2, 2));
    vec3 tri_offset = vec3();
    int cam_to_use = 0;

    // Create shaders
    BlinnPhongShader bp_frag = BlinnPhongShader();
    TextureShader tex_frag = TextureShader();

    ColorShader blue_frag = ColorShader(vec3(0, 0, 1));
    ColorShader red_frag = ColorShader(vec3(1, 0, 0));
    ColorShader green_frag = ColorShader(vec3(0, 1, 0));

    Shader shader = Shader();
    shader.frag = &bp_frag;

    // Set up time
    double current_time = 0;
    double prev_time = 0;
    double delta_time = 0;

    double start_time = GetCurrentTime();

    int seconds = 0; // Number of seconds which have passed

    // Rendering Loop
    SDL_Event event;
    while(true){
        // Poll for input
        if (HandleInput(tri_offset, cam_to_use, &secondary_cam, delta_time, event) == -1) break;

        // Update time
        prev_time = current_time;
        current_time = GetCurrentTime() - start_time;
        delta_time = current_time - prev_time;

        // Prints FPS once every second to keep track of performance
        if (int(current_time) - seconds == 0){
            int fps = int(1.0f/delta_time);
            printf("FPS: %i\n", fps);

            seconds++;
        }

        // Clear buffer
        render_buffer.Clear(vec3(0, 0, 0));

        // Render geometry
        mat4 tri_model = GetModelMatrix(vec3(0.0f, 0.1f, 0.0f) + tri_offset, vec3(5.0f, 5.0f, 5.0f), 0, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));

        v0 = vec4(-0.5f, 0.0f, -0.5f);
        v1 = vec4(0.5f, 0.0f, -0.5f);
        v2 = vec4(0.0f, 0.0f, 0.5f);
        Triangle3D clip_tri = Triangle3D(tri_model * v0, tri_model * v1, tri_model * v2);
        
        clip_tri.vertices[0].position.w = clip_tri.vertices[0].position.z;
        clip_tri.vertices[1].position.w = clip_tri.vertices[1].position.z;
        clip_tri.vertices[2].position.w = clip_tri.vertices[2].position.z;

        std::vector<Triangle3D> clip_result = ClipTriangle(&clip_tri);

        shader.frag = &green_frag;
        for (Triangle3D clipped_tri : clip_result){
            clipped_tri.vertices[0].position.w = 1.0f;
            clipped_tri.vertices[1].position.w = 1.0f;
            clipped_tri.vertices[2].position.w = 1.0f;

            DrawTriangle(clipped_tri, render_buffer, proj * view, tex, shader);
        }

        DrawLine(vec4(), vec4(20, 0, 20, 1), render_buffer, proj * view, vec3(0, 0, 1));
        DrawLine(vec4(-20, 0, 0, 1), vec4(20, 0, 0, 1), render_buffer, proj * view, vec3(0, 0, 1));
        DrawLine(vec4(), vec4(-20, 0, 20, 1), render_buffer, proj * view, vec3(0, 0, 1));

        // Empty buffer to Renderer
        BlitBuffer(render_buffer, sdl_buffer, renderer);

        // Output to screen
        SDL_RenderPresent(renderer);
    }

    // End program
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}