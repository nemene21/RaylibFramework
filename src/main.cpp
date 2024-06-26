#include <iostream>
#include <time.h>
#include <framework.h>

void process(float delta) {
    // Processing data managers
    TextureManager::unload_check();
    ParticleDataManager::unload_check();
    
    AudioManager::unload_check();
    AudioManager::process_track_fade(delta);

    ShaderManager::unload_check();
    ShaderManager::update_uniforms();

    ColliderManager::reload_colliders();
    AreaManager::reload_areas();

    // Hot reloading
    if (TryingToHotReload()) {
        TextureManager::reload();
        ShaderManager::reload();
        AudioManager::reload();
        ParticleDataManager::reload();
    }

    SceneManager::scene_on->process(delta);
    SceneManager::scene_on->process_entities(delta);
}

void draw_background(float delta) {

}

void draw_foreground(float delta) {
    SceneManager::scene_on->draw_entities(delta);
    DrawableManager::draw();
}

int main() {
    srand(time(NULL));

    // Init
    std::string title = "ne_mene's framework :)";
    InitWindow(res.x, res.y, (title + "  -  [F11 for fullscreen]").c_str());
    SetWindowState(FLAG_WINDOW_RESIZABLE);
    SetExitKey(0);

    RenderTexture2D background  = LoadRenderTexture(res.x, res.y);
    RenderTexture2D foreground  = LoadRenderTexture(res.x, res.y);
    RenderTexture2D composition = LoadRenderTexture(res.x, res.y);
    SetTargetFPS(120);

    InitAudioDevice();

    Camera2D blank_camera;
    blank_camera.target = {0, 0};
    blank_camera.offset = {0, 0};
    blank_camera.rotation = 0;
    blank_camera.zoom = 1;

    Easing::InitEasingFuncs();
    CameraManager::init();
    ColliderManager::init();
    AreaManager::init();

    CameraManager::bind_camera(&blank_camera);

    ShaderPtr post_processing_ptr = ShaderManager::get("post_processing.glsl"); 
    TexturePtr noise_texture = TextureManager::get("post_processing/noise.png");
    TexturePtr paper_texture = TextureManager::get("post_processing/paper.png");

    new GameScene();
    SceneManager::set_scene("game_scene");

    while (!WindowShouldClose()) {
        float delta = fminf(GetFrameTime(), 1/20.f);

        if (IsWindowResized()) {
            SetWindowSize(GetScreenWidth(), GetScreenHeight());
        }
  
        // Object processing test
        process(delta);

        // Drawing start
        BeginDrawing();

        // Background
        BeginTextureMode(background);

        ClearBackground(Color{0, 0, 0, 255});
        draw_background(delta);

        EndTextureMode();

        // Foreground
        BeginTextureMode(foreground);
        ClearBackground({0, 0, 0, 0});

        if (global_camera != nullptr) {
            BeginMode2D(*global_camera);
        }

        // Object drawing test
        EndShaderMode();

        draw_foreground(delta);
        ColliderManager::draw_debug();
        AreaManager::draw_debug();

        EndMode2D();        
        EndTextureMode();
        BeginTextureMode(composition);

        DrawTexturePro(background.texture,
            {0, 0, res.x, res.y},
            {0, 0, res.x, res.y},
            {0, 0},
            0,
            WHITE
        );

        DrawTexturePro(foreground.texture,
            {0, 0, res.x, res.y},
            {0, 0, res.x, res.y},
            {0, 0},
            0,
            WHITE
        );

        EndTextureMode();

        float timer = GetTime();
        Shader post_processing = *post_processing_ptr.get();
        BeginShaderMode(post_processing);

        SetShaderValue(post_processing, GetShaderLocation(post_processing, "time"), &timer, SHADER_UNIFORM_FLOAT);
        
        SetShaderValueTexture(post_processing, GetShaderLocation(post_processing, "noise_texture"), *noise_texture.get());
        SetShaderValueTexture(post_processing, GetShaderLocation(post_processing, "paper_texture"), *paper_texture.get());

        if (global_camera != nullptr) {
            Vector2 camera_offset = Vector2Divide(global_camera->target, res);

            SetShaderValue(
                post_processing,
                GetShaderLocation(post_processing, "camera_offset"),
                &camera_offset,
                SHADER_ATTRIB_VEC2
            );
        }

        float draw_width  = GetScreenWidth();
        float draw_height = GetScreenWidth() * (res.y / res.x);

        if (draw_height > GetScreenHeight()) {
            draw_height = GetScreenHeight();
            draw_width  = GetScreenHeight() * (res.x / res.y);
        }

        ClearBackground(BLACK);
        DrawTexturePro(composition.texture,
            {0, 0, res.x, res.y},
            {(GetScreenWidth() - draw_width) * .5f, (GetScreenHeight() - draw_height) * .5f, draw_width, draw_height},
            {0, 0},
            0,
            Color{255, 255, 255}
        );
        EndShaderMode();

        EndDrawing();

        // FPS counter
        std::string fps_string = std::to_string(GetFPS());
        const char* fps_cstring = fps_string.c_str();
        SetWindowTitle(fps_cstring);
    }

    // Unload remaining assets
    TextureManager::unload_all();
    ParticleDataManager::unload_all();
    ShaderManager::unload_all();
    SceneManager::unload_all();
    AudioManager::unload_all();

    CloseWindow();
    return 0;
}