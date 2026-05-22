#include <iostream>
#include <math.h>
#include <sys/time.h>

#include <renderer/renderer.h>
#include <SDL3/SDL.h>

const int SCR_WIDTH = 800/4, SCR_HEIGHT = 600/4; // Pixels to render
const int RENDER_SCALE = 4; // Multiplier for the actual scale of the window

const float MOVE_SPEED = 10.0f;
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
            SDL_RenderPoint(renderer, j, buffer.h-i);
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

int HandleInput(vec3 &offset, Camera *cam, float delta_time, SDL_Event event){
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
                offset += vec3(-velocity, 0, 0);
            } else if (event.key.key == SDLK_RIGHT){
                offset += vec3(velocity, 0, 0);
            } else if (event.key.key == SDLK_UP){
                offset += vec3(0, velocity, 0);
            } else if (event.key.key == SDLK_DOWN){
                offset += vec3(0, -velocity, 0);
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
    vec4 v0 = vec4(-0.5f, -0.5f, 0.0f);
    vec4 v1 = vec4(0.5f, -0.5f, 0.0f);
    vec4 v2 = vec4(0.0f, 0.5f, 0.0f);

    // Projection matrix
    mat4 proj = GetProjectionMatrix(45, 4.0f/3.0f, 0.1f, 100.0f);

    // Create Camera
    Camera main_cam = Camera(vec3(0, 24, 0), vec3(0, 0, 1), vec3(0, -1, 0));
    Camera secondary_cam = Camera(vec3(), vec3(0, 1, 0), vec3(0, 0, -1));

    vec3 tri_offset = vec3();

    // Load models
    Mesh cube = Mesh("assets/models/cube.obj");
    Texture tex = Texture("assets/images/worldsky.png");
    cube.LinkTexture(tex);

    aabb test_box = aabb(vec3(0, 0, -8), vec3(2, 2, 2));

    // Create shaders
    TextureShader fragment = TextureShader();

    ColorShader blue_frag = ColorShader(vec3(0, 0, 1));
    ColorShader red_frag = ColorShader(vec3(1, 0, 0));
    ColorShader green_frag = ColorShader(vec3(0, 1, 0));

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
        if (HandleInput(tri_offset, &secondary_cam, delta_time, event) == -1) break;

        // Update time
        prev_time = current_time;
        current_time = GetCurrentTime() - start_time;
        delta_time = current_time - prev_time;

        // Prints FPS once every second to keep track of performance
        if (int(current_time) - seconds == 0){
            int fps = int(1.0f/delta_time);
            printf("FPS: %i\n", fps);

            std::cout << "Camera Position: " << main_cam.position << '\n';
            std::cout << "Camera Right: " << main_cam.right << '\n';
            std::cout << "Camera Up: " << main_cam.up << '\n';
            std::cout << "Camera Forward: " << main_cam.forward << '\n';
            std::cout << "Camera Direction: " << normalize(-main_cam.forward) << '\n';

            seconds++;
        }

        // Clear buffer
        render_buffer.Clear(vec3(0, 0, 0));

        // Render geometry
        mat4 tri_model = GetModelMatrix(vec3(0.0f, 0.0f, -2.0f) + tri_offset, vec3(1.0f, 1.0f, 1.0f), 0, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));
        mat4 cube_model = GetModelMatrix(vec3(0.0f, 0.0f, -7.0f), vec3(1.0f, 1.0f, 1.0f), current_time, vec3(1.0f, 0.0f, 0.5f), vec3(0.0f, 0.0f, 0.0f));
        mat4 cube_model_2 = GetModelMatrix(vec3(0.0f, 0.0f, 7.0f), vec3(1.0f, 1.0f, 1.0f), current_time, vec3(1.0f, 0.0f, 0.5f), vec3(0.0f, 0.0f, 0.0f));
        mat4 cube_model_3 = GetModelMatrix(vec3(7.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), current_time, vec3(1.0f, 0.0f, 0.5f), vec3(0.0f, 0.0f, 0.0f));
        mat4 cube_model_4 = GetModelMatrix(vec3(-7.0f, 0.0f, 0.0f), vec3(1.0f, 1.0f, 1.0f), current_time, vec3(1.0f, 0.0f, 0.5f), vec3(0.0f, 0.0f, 0.0f));
        mat4 line_model = GetRotationMatrix(length(tri_offset), vec3(0, 0, 1));

        mat4 view = secondary_cam.GetViewMatrix();

        Triangle3D tri = Triangle3D(tri_model * v0, tri_model * v1, tri_model * v2);

        shader.frag = &fragment;
        DrawMesh(cube, render_buffer, proj * view, cube_model, shader);
        shader.frag = &blue_frag;
        DrawMesh(cube, render_buffer, proj * view, cube_model_2, shader);
        shader.frag = &red_frag;
        DrawMesh(cube, render_buffer, proj * view, cube_model_3, shader);
        shader.frag = &green_frag;
        DrawMesh(cube, render_buffer, proj * view, cube_model_4, shader);
        
        //DrawLine(vec4(), v3tov4(secondary_cam.forward*4.0f, 1.0f), render_buffer, proj * view, vec3(0, 0, 1));
        //DrawLine(vec4(), v3tov4(secondary_cam.right*4.0f, 1.0f), render_buffer, proj * view, vec3(1, 0, 0));
        //DrawLine(vec4(), v3tov4(secondary_cam.up*4.0f, 1.0f), render_buffer, proj * view, vec3(0, 1, 0));

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