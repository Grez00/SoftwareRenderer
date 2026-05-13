#include <iostream>
#include <math.h>
#include <sys/time.h>

#include <renderer/renderer.h>
#include <SDL3/SDL.h>

const int SCR_WIDTH = 800/4, SCR_HEIGHT = 600/4; // Pixels to render
const int RENDER_SCALE = 4; // Multiplier for the actual scale of the window

const float MOVE_SPEED = 20.0f;
const float ROTATION_SPEED = 5.0f;

// Get the time of day in seconds
double GetCurrentTime(){
    struct timeval tv;

    gettimeofday(&tv,NULL);
    return ((double) tv.tv_sec) + ((double) tv.tv_usec / 1000000.0);
}

// Output the contents of a RenderBuffer to an SDL_Renderer
void BlitBuffer(FrameBuffer buffer, SDL_Renderer *renderer){
    for (int i = 0; i < buffer.h; i++){
        for (int j = 0; j < buffer.w; j++){
            SDL_SetRenderDrawColor(
                renderer, 
                clamp(buffer.render_buffer[j][i].x * 255.0f, 0.0f, 255.0f), 
                clamp(buffer.render_buffer[j][i].y * 255.0f, 0.0f, 255.0f), 
                clamp(buffer.render_buffer[j][i].z * 255.0f, 0.0f, 255.0f), 
                SDL_ALPHA_OPAQUE
            );
            SDL_RenderPoint(renderer, j, i);
        }
    }
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

int HandleInput(Camera *cam, float delta_time, SDL_Event event){
    if (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT){
            return -1;
        }
        else if (event.type == SDL_EVENT_KEY_DOWN){
            float velocity = MOVE_SPEED * delta_time;
            if (event.key.key == SDLK_W) {
                cam->position += cam->forward * velocity;
            } else if (event.key.key == SDLK_A) {
                cam->position += cam->right * velocity;
            } else if (event.key.key == SDLK_S) {
                cam->position -= cam->forward * velocity;
            } else if (event.key.key == SDLK_D) {
                cam->position -= cam->right * velocity;
            } else if (event.key.key == SDLK_LEFT){

            } else if (event.key.key == SDLK_RIGHT){

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

    // Create Frame Buffer
    FrameBuffer render_buffer = FrameBuffer(SCR_WIDTH, SCR_HEIGHT);

    // Set up geometry
    vec4 v0 = vec4(-0.5f, 0.0f, 0.0f);
    vec4 v1 = vec4(0.5f, 0.0f, 0.0f);
    vec4 v2 = vec4(0.0f, 1.0f, 0.0f);

    // Projection matrix
    mat4 proj = GetProjectionMatrix(45, 4.0f/3.0f, 0.1f, 100.0f);

    // Create Camera
    Camera main_cam = Camera();

    // Load models
    Mesh cube = Mesh("assets/models/cube.obj");
    Texture tex = Texture("assets/images/worldsky.png");
    cube.LinkTexture(tex);

    // Create shaders
    BlinnPhongShader fragment = BlinnPhongShader();
    Shader shader = Shader();
    shader.frag = &fragment;

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
        if (HandleInput(&main_cam, delta_time, event) == -1) break;

        // Update time
        prev_time = current_time;
        current_time = GetCurrentTime() - start_time;
        delta_time = current_time - prev_time;

        // Prints FPS once every second to keep track of performance
        if (int(current_time) - seconds == 0){
            int fps = int(1.0f/delta_time);
            printf("FPS: %i\n", fps);
            printf("Camera Position: ");
            std::cout << main_cam.position << '\n';

            seconds++;
        }

        // Clear buffer
        render_buffer.Clear(vec3(0, 0, 0));

        // Render geometry
        //mat4 tri_model = GetModelMatrix(vec3(-0.1f, -0.5f, 2.0f), vec3(1.0f, 1.0f, 1.0f), 180, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.5f, 0.0f));
        mat4 cube_model = GetModelMatrix(vec3(0.0f, 0.0f, 5.0f), vec3(1.0f, 1.0f, 1.0f), current_time, vec3(1.0f, 0.0f, 0.5f), vec3(0.0f, 0.0f, 0.0f));

        DrawMesh(cube, render_buffer, proj, main_cam.GetViewMatrix() * cube_model, shader);

        // Empty buffer to Renderer
        BlitBuffer(render_buffer, renderer);

        // Output to screen
        SDL_RenderPresent(renderer);
    }

    // End program
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}