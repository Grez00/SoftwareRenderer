#include <iostream>
#include <vector>
#include <math.h>
#include <sys/time.h>
#include <omp.h>

#include <renderer/renderer.h>
#include <SDL3/SDL.h>

#define PI 3.14159265358979323846f

const int SCR_WIDTH = 800/2, SCR_HEIGHT = 600/2; // Pixels to render
const float RENDER_SCALE = 2.5f; // Multiplier for the actual scale of the window

const float MOVE_SPEED = 5.0f;
const float ROTATION_SPEED = 2.5f;

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

void BlitTexture(Texture tex, SDL_Renderer *renderer, SAMPLE_TYPE type){
    for (int i = 0; i < SCR_HEIGHT; i++){
        for (int j = 0; j < SCR_WIDTH; j++){
            vec3 sampled_col = tex.sample(vec2(float(j)/float(SCR_WIDTH), float(i)/float(SCR_HEIGHT)), type);
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

int HandleInput(vec3 &offset, int &cam_to_use, Camera *cam, double delta_time, SDL_Event event){
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
                cam->forward = normalize(GetRotationMatrix(rotation_velocity, cam->right) * cam->forward);
            } else if (event.key.key == SDLK_F) {
                cam->forward = normalize(GetRotationMatrix(-rotation_velocity, cam->right) * cam->forward);
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
    Triangle3D test_tri = Triangle3D(v0, v1, v2);

    // Create Camera
    Camera main_cam = Camera(vec3(0, 24, 0), vec3(0, 0, -1), vec3(0, -1, 0));
    Camera secondary_cam = Camera(vec3(), vec3(0, 1, 0), vec3(0, 0, -1));

    // Create matrices
    mat4 view = secondary_cam.view;
    mat4 proj = secondary_cam.proj;

    // Load models
    Mesh cube = Mesh("assets/models/cube.obj");
    Mesh bishop = Mesh("assets/models/bishop.obj");
    Mesh icosphere = Mesh("assets/models/icosphere.obj");
    Mesh sphere_mesh = Mesh("assets/models/sphere_smooth.obj");
    Mesh king = Mesh("assets/models/king.obj");
    Mesh skybox_cube = Mesh("assets/models/skybox_cube.obj");

    // Load Textures
    Texture tex = Texture("assets/images/image.jpg");
    CubeMap skybox_tex = CubeMap(
        "assets/images/skybox_1/right.jpg",
        "assets/images/skybox_1/left.jpg",
        "assets/images/skybox_1/top.jpg",
        "assets/images/skybox_1/bottom.jpg",
        "assets/images/skybox_1/front.jpg",
        "assets/images/skybox_1/back.jpg"
    );

    aabb test_box = aabb(vec3(0, 0, -7), vec3(2, 2, 2));
    vec3 tri_offset = vec3();
    int cam_to_use = 0;
    float angle = 0;

    // Set up Lighting
    dirlight dir_light_1 = dirlight(vec3(1, 0, 0), vec3(0.1, 0.1, 0.1), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f));
    dirlight dir_light_2 = dirlight(vec3(-1, 0, 0), vec3(0.1, 0.1, 0.1), vec3(1.0f, 1.0f, 1.0f), vec3(1.0f, 1.0f, 1.0f));
    pointlight p_light_1 = pointlight(vec3(-2.0f, 0.0f, -5.0f), 0.1f, 0.1f, vec3(0.2f, 0.2f, 0.2f), vec3(1.0f, 0.0f, 0.0f), vec3(1.0f, 0.0f, 0.0f));
    pointlight p_light_2 = pointlight(vec3(2.0f, 0.0f, -5.0f), 0.1f, 0.1f, vec3(0.2f, 0.2f, 0.2f), vec3(0.0f, 0.0f, 1.0f), vec3(0.0f, 0.0f, 1.0f));
    dirlight *dir_lights = new dirlight[2];
    dir_lights[0] = dir_light_1;
    dir_lights[1] = dir_light_2;
    pointlight *point_lights = new pointlight[2];
    point_lights[0] = p_light_1;
    point_lights[1] = p_light_2;
    SceneLighting lighting_info = SceneLighting(dir_lights, point_lights, 0, 2);

    // Create shaders
    BlinnPhongShader bp_frag = BlinnPhongShader();
    SkyboxShader skybox_shader = SkyboxShader(&skybox_tex);
    ColorShader color_shader = ColorShader(vec3(1, 0, 0));

    skybox_shader.light_info = &lighting_info;

    bp_frag.map_albedo = &tex;
    bp_frag.light_info = &lighting_info;
    bp_frag.tint = vec3(1.0f, 1.0f, 1.0f);
    bp_frag.smoothness = 0.5f;
    bp_frag.metallic = 0.0f;

    color_shader.light_info = &lighting_info;

    Model painted_sphere = Model("assets/models/PaintedSphere.obj");
    Model cat_cube = Model("assets/models/catcube.obj");

    // Set up time
    double current_time = 0;
    double prev_time = 0;
    double delta_time = 0;

    double start_time = GetCurrentTime();

    int seconds = 0; // Number of seconds which have passed

    // Rendering Loop
    SDL_Event event;
    while(true){
        // Update time
        prev_time = current_time;
        current_time = GetCurrentTime() - start_time;
        delta_time = current_time - prev_time;

        // Poll for input
        if (HandleInput(tri_offset, cam_to_use, &secondary_cam, delta_time, event) == -1) break;

        // Prints FPS once every second to keep track of performance
        if (int(current_time) - seconds == 0){
            int fps = int(1.0f/delta_time);
            printf("FPS: %i\n", fps);

            seconds++;
        }

        // Clear buffer
        render_buffer.Clear(vec3(0, 0, 0));

        // Render geometry
        mat4 cube_model = 
            GetModelMatrix(vec3(0.0f, 0.0f, -5.0f) + tri_offset, vec3(1.0f, 1.0f, 1.0f), current_time, vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));

        //mat4 light_model = 
            //GetModelMatrix(vec3(0.0f, 2.0f, -5.0f), vec3(0.1f, 0.1f, 0.1f), 0.0f, vec3(1.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, 0.0f));

        view = secondary_cam.view;
        proj = secondary_cam.proj;

        bp_frag.view = view;
        bp_frag.proj = proj;
        bp_frag.model = cube_model;

        color_shader.view = mat3tomat4(mat3(view));
        color_shader.proj = proj;
        //color_shader.model = cube_model;

        skybox_shader.view = mat3tomat4(mat3(view));
        skybox_shader.proj = proj;
        DrawMesh(skybox_cube, &render_buffer, &color_shader, false);

        //painted_sphere.shaders->SetSceneInfo(cube_model, view, proj, secondary_cam.position, &lighting_info);
        //DrawModel(painted_sphere, &render_buffer);

        //DrawMesh(king, &render_buffer, &bp_frag);
        //DrawMeshWireframe(sphere_mesh, &render_buffer, proj, view * cube_model, vec3(0, 1, 0));

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