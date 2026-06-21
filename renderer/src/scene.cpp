#include "renderer/scene.h"

int PrintNestedVector(std::vector<std::any> vector){
    std::cout << '[';
    for (int i = 0; i < vector.size(); i++){
        std::any value = vector[i];
        if (auto val = std::any_cast<std::string>(&value)){
            std::cout << *val;
        }
        else if (auto val = std::any_cast<float>(&value)){
            std::cout << *val;
        }
        else if (auto val = std::any_cast<std::map<std::string, std::any>>(&value)){
            PrintNestedMap(*val);
        }
        else if (auto val = std::any_cast<std::vector<std::any>>(&value)){
            PrintNestedVector(*val);
        }
        else{
            return -1;
        }

        if (i != vector.size()-1){
            std::cout << ", ";
        }
    }
    std::cout << "]\n";

    return 0;
}

int PrintNestedMap(std::map<std::string, std::any> map){
    std::cout << '{';
    for (auto const& [key, value] : map){
        if (auto val = std::any_cast<std::string>(&value)){
            std::cout << key << " : " << *val << '\n';
        }
        else if (auto val = std::any_cast<float>(&value)){
            std::cout << key << " : " << *val << '\n';
        }
        else if (auto val = std::any_cast<std::map<std::string, std::any>>(&value)){
            std::cout << key << " : ";
            PrintNestedMap(*val);
        }
        else if (auto val = std::any_cast<std::vector<std::any>>(&value)){
            std::cout << key << " : ";
            PrintNestedVector(*val);
        }
        else{
            return -1;
        }
    }
    std::cout << "}\n";

    return 0;
}

Scene::Scene(const std::string &filename){
    std::map<std::string, std::any> result = ParseJSONFile(filename);
    PrintNestedMap(result);

    // CAMERA
    if (auto cam = std::any_cast<std::map<std::string, std::any>>(&result["camera"])){
        vec3 position;
        vec3 up;
        vec3 lookat;
        float fov;
        float aspect_ratio;
        float near;
        float far;

        if (auto p_position = std::any_cast<std::vector<std::any>>(&(*cam)["position"])){
            position = vec3(
                std::any_cast<float>((*p_position)[0]),
                std::any_cast<float>((*p_position)[1]),
                std::any_cast<float>((*p_position)[2])
            );
        }
        if (auto p_up = std::any_cast<std::vector<std::any>>(&(*cam)["up"])){
            up = vec3(
                std::any_cast<float>((*p_up)[0]),
                std::any_cast<float>((*p_up)[1]),
                std::any_cast<float>((*p_up)[2])
            );
        }
        if (auto p_lookat = std::any_cast<std::vector<std::any>>(&(*cam)["lookat"])){
            lookat = vec3(
                std::any_cast<float>((*p_lookat)[0]),
                std::any_cast<float>((*p_lookat)[1]),
                std::any_cast<float>((*p_lookat)[2])
            );
        }
        if (auto p_fov = std::any_cast<float>(&(*cam)["fov"])){
            fov = *p_fov;
        }
        if (auto p_aspect = std::any_cast<float>(&(*cam)["aspect_ratio"])){
            aspect_ratio = *p_aspect;
        }
        if (auto p_near = std::any_cast<float>(&(*cam)["near"])){
            near = *p_near;
        }
        if (auto p_far = std::any_cast<float>(&(*cam)["far"])){
            far = *p_far;
        }

        main_cam = Camera(
            position,
            up,
            normalize(lookat - position),
            fov,
            aspect_ratio,
            near,
            far
        );
    }
    else{
        printf("Scene: Error, invalid/missing camera data\n");
    }

    // LIGHTING
    if (auto lighting = std::any_cast<std::map<std::string, std::any>>(&result["lighting"])){
        std::vector<std::any> p_dir_lights = std::any_cast<std::vector<std::any>>((*lighting)["dir_lights"]);
        dirlight *dir_lights = new dirlight[p_dir_lights.size()];
        for (int i = 0; i < p_dir_lights.size(); i++){
            std::map<std::string, std::any> next_light = std::any_cast<std::map<std::string, std::any>>(p_dir_lights[i]);

            vec3 direction;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;

            if (auto p_dir = std::any_cast<std::vector<std::any>>(&next_light["direction"])){
                direction = vec3(
                    std::any_cast<float>((*p_dir)[0]),
                    std::any_cast<float>((*p_dir)[1]),
                    std::any_cast<float>((*p_dir)[2])
                );
            }
            if (auto p_ambient = std::any_cast<std::vector<std::any>>(&next_light["ambient"])){
                ambient = vec3(
                    std::any_cast<float>((*p_ambient)[0]),
                    std::any_cast<float>((*p_ambient)[1]),
                    std::any_cast<float>((*p_ambient)[2])
                );
            }
            if (auto p_diffuse = std::any_cast<std::vector<std::any>>(&next_light["diffuse"])){
                diffuse = vec3(
                    std::any_cast<float>((*p_diffuse)[0]),
                    std::any_cast<float>((*p_diffuse)[1]),
                    std::any_cast<float>((*p_diffuse)[2])
                );
            }
            if (auto p_specular = std::any_cast<std::vector<std::any>>(&next_light["specular"])){
                specular = vec3(
                    std::any_cast<float>((*p_specular)[0]),
                    std::any_cast<float>((*p_specular)[1]),
                    std::any_cast<float>((*p_specular)[2])
                );
            }

            dir_lights[i] = dirlight(
                direction,
                ambient,
                diffuse,
                specular
            );
        }

        std::vector<std::any> p_point_lights = std::any_cast<std::vector<std::any>>((*lighting)["point_lights"]);
        pointlight *point_lights = new pointlight[p_point_lights.size()];
        for (int i = 0; i < p_point_lights.size(); i++){
            std::map<std::string, std::any> next_light = std::any_cast<std::map<std::string, std::any>>(p_point_lights[i]);

            vec3 position;
            vec3 ambient;
            vec3 diffuse;
            vec3 specular;

            float linear;
            float quadratic;

            if (auto p_position = std::any_cast<std::vector<std::any>>(&next_light["position"])){
                position = vec3(
                    std::any_cast<float>((*p_position)[0]),
                    std::any_cast<float>((*p_position)[1]),
                    std::any_cast<float>((*p_position)[2])
                );
            }
            if (auto p_linear = std::any_cast<float>(&next_light["linear"])){
                linear = *p_linear;
            }
            if (auto p_quadratic = std::any_cast<float>(&next_light["quadratic"])){
                quadratic = *p_quadratic;
            }
            if (auto p_ambient = std::any_cast<std::vector<std::any>>(&next_light["ambient"])){
                ambient = vec3(
                    std::any_cast<float>((*p_ambient)[0]),
                    std::any_cast<float>((*p_ambient)[1]),
                    std::any_cast<float>((*p_ambient)[2])
                );
            }
            if (auto p_diffuse = std::any_cast<std::vector<std::any>>(&next_light["diffuse"])){
                diffuse = vec3(
                    std::any_cast<float>((*p_diffuse)[0]),
                    std::any_cast<float>((*p_diffuse)[1]),
                    std::any_cast<float>((*p_diffuse)[2])
                );
            }
            if (auto p_specular = std::any_cast<std::vector<std::any>>(&next_light["specular"])){
                specular = vec3(
                    std::any_cast<float>((*p_specular)[0]),
                    std::any_cast<float>((*p_specular)[1]),
                    std::any_cast<float>((*p_specular)[2])
                );
            }

            point_lights[i] = pointlight(
                position,
                linear,
                quadratic,
                ambient,
                diffuse,
                specular
            );
        }

        lighting_info = SceneLighting(dir_lights, point_lights, p_dir_lights.size(), p_point_lights.size());
    }
    else{
        printf("Scene: Error, invalid/missing lighting data\n");
    }

    // TEXTURES
    if (auto p_textures = std::any_cast<std::vector<std::any>>(&result["textures"])){
        num_textures = (*p_textures).size();
        textures = new Texture[num_textures];
        for (int i = 0; i < (*p_textures).size(); i++){
            textures[i] = Texture(
                std::any_cast<std::string>(std::any_cast<std::map<std::string, std::any>>((*p_textures)[i])["path"])
            );
        }
    }
    else{
        printf("Scene: Error, invalid/missing texture data\n");
    }

    // MATERIALS
    if (auto p_materials = std::any_cast<std::vector<std::any>>(&result["materials"])){
        num_mats = (*p_materials).size();
        materials = new Material[num_mats];
        for (int i = 0; i < (*p_materials).size(); i++){
            std::map<std::string, std::any> next_material = std::any_cast<std::map<std::string, std::any>>((*p_materials)[i]);
            std::map<std::string, std::any> next_vert = std::any_cast<std::map<std::string, std::any>>(next_material["vert"]);
            std::map<std::string, std::any> next_frag = std::any_cast<std::map<std::string, std::any>>(next_material["frag"]);

            VertexShader vert;
            FragmentShader frag;
            
            std::string vert_type = std::any_cast<std::string>(next_vert["type"]);
            if (vert_type == "standard"){
                vert = StandardVertex();
            }

            std::string frag_type = std::any_cast<std::string>(next_frag["type"]);
            if (frag_type == "blinn_phong"){
                frag = BlinnPhongShader(
                    &textures[(int)std::any_cast<float>(next_frag["texture"])],
                    &lighting_info,
                    main_cam.position
                );
            }
            else if (frag_type == "texture"){
                frag = TextureShader(
                    &textures[(int)std::any_cast<float>(next_frag["texture"])]
                );
            }
            else if (frag_type == "normal"){
                frag = NormalShader();
            }
            else if (frag_type == "color"){
                std::vector<std::any> col_vector = std::any_cast<std::vector<std::any>>(next_frag["color"]);
                frag = ColorShader(
                    vec3(
                        std::any_cast<float>(col_vector[0]),
                        std::any_cast<float>(col_vector[1]),
                        std::any_cast<float>(col_vector[2]) 
                    )
                );
            }

            Shader shader = Shader(&vert, &frag);
            materials[i] = Material(&shader);
        }
    }
    else{
        printf("Scene: Error, invalid/missing material data\n");
    }

    // MODELS
    if (auto p_models = std::any_cast<std::vector<std::any>>(&result["models"])){
        num_models = (*p_models).size();
        models = new Model[num_models];
        for (int i = 0; i < (*p_models).size(); i++){
            std::map<std::string, std::any> next_model = std::any_cast<std::map<std::string, std::any>>((*p_models)[i]);
            std::map<std::string, std::any> next_matrix = std::any_cast<std::map<std::string, std::any>>(next_model["matrix"]);

            vec3 position;
            vec3 scale;
            float rotation;
            vec3 axis;
            vec3 r_point;

            if (auto p_position = std::any_cast<std::vector<std::any>>(&next_matrix["position"])){
                position = vec3(
                    std::any_cast<float>((*p_position)[0]),
                    std::any_cast<float>((*p_position)[1]),
                    std::any_cast<float>((*p_position)[2])
                );
            }
            if (auto p_scale = std::any_cast<std::vector<std::any>>(&next_matrix["scale"])){
                scale = vec3(
                    std::any_cast<float>((*p_scale)[0]),
                    std::any_cast<float>((*p_scale)[1]),
                    std::any_cast<float>((*p_scale)[2])
                );
            }
            if (auto p_rotation = std::any_cast<float>(&next_matrix["rotation"])){
                rotation = *p_rotation;
            }
            if (auto p_axis = std::any_cast<std::vector<std::any>>(&next_matrix["axis"])){
                axis = vec3(
                    std::any_cast<float>((*p_axis)[0]),
                    std::any_cast<float>((*p_axis)[1]),
                    std::any_cast<float>((*p_axis)[2])
                );
            }
            if (auto p_r_point = std::any_cast<std::vector<std::any>>(&next_matrix["r_point"])){
                r_point = vec3(
                    std::any_cast<float>((*p_r_point)[0]),
                    std::any_cast<float>((*p_r_point)[1]),
                    std::any_cast<float>((*p_r_point)[2])
                );
            }

            Mesh mesh = Mesh(std::any_cast<std::string>(next_model["path"]));

            models[i] = Model(
                &mesh,
                &materials[(int)std::any_cast<float>(next_model["material"])],
                GetModelMatrix(
                    position,
                    scale,
                    rotation,
                    axis,
                    r_point
                )
            );
        }
    }
    else{
        printf("Scene: Error, invalid/missing model data\n");
    }
}

void Scene::Draw(FrameBuffer *buffer){
    frustum cam_frustum = frustum(main_cam);
    for (int i = 0; i < num_models; i++){
        // Frustum Culling
        sphere bounding_sphere = models[i].mesh->GetBoundingSphere();
        bounding_sphere.c = models[i].model * v3tov4(bounding_sphere.c, 1.0f);
        if (!FrustumSphereIntersect(cam_frustum, bounding_sphere)) continue;

        DrawMesh(*models[i].mesh, buffer, models[i].mat->shader);
    }
}