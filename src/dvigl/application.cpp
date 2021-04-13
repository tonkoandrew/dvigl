#include <dvigl/application.h>
#include <dvigl/audio_manager.h>
#include <dvigl/file_system_manager.h>
#include <dvigl/font_manager.h>
#include <dvigl/input_manager.h>
#include <dvigl/lod_group_manager.h>
#include <dvigl/material_manager.h>
#include <dvigl/model_manager.h>
#include <dvigl/network_manager.h>
#include <dvigl/particle_system_manager.h>
// #include <dvigl/physics_manager.h>
#include <dvigl/render_manager.h>
#include <dvigl/scene_manager.h>
#include <dvigl/shader_manager.h>
#include <dvigl/texture_manager.h>
#include <dvigl/video_manager.h>

#include <dvigl/camera_node.h>
#include <dvigl/scene.h>

Application gApplication;

static void DoMath(glm::mat4& transform) {}

bool Application::init()
{
    error_code = 0;

    int result = SDL_Init(SDL_INIT_EVERYTHING);

    if (result)
    {
        LOG("SDL_Init failed: %s\n", SDL_GetError());
        error_code = 1;
        return false;
    }

    LOG("initializing FileSystemMgr... ");
    if (!FileSystemMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 3;
        return false;
    }
    LOG("done\n");

    LOG("initializing InputMgr... ");
    if (!InputMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 5;
        return false;
    }
    LOG("done\n");

    LOG("initializing RenderMgr... ");
    if (!RenderMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 6;
        return false;
    }
    LOG("done\n");

    LOG("initializing ShaderMgr... ");
    if (!ShaderMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 16;
        return false;
    }
    LOG("done\n");

    LOG("initializing TextureMgr... ");
    if (!TextureMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 14;
        return false;
    }
    LOG("done\n");

    LOG("initializing MaterialMgr... ");
    if (!MaterialMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 15;
        return false;
    }
    LOG("done\n");

    LOG("initializing AudioMgr... ");
    if (!AudioMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 11;
        return false;
    }
    LOG("done\n");

    // LOG("initializing VideoMgr... ");
    // if (!VideoMgr::ptr()->init())
    // {
    //     LOG("failed\n");
    //     error_code = 15;
    //     return false;
    // }
    // LOG("done\n");

    LOG("initializing FontMgr... ");
    if (!FontMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 13;
        return false;
    }
    LOG("done\n");

    LOG("initializing ParticleSystemMgr... ");
    if (!ParticleSystemMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 13;
        return false;
    }
    LOG("done\n");

    // LOG("initializing PhysicsMgr... ");
    // if (!PhysicsMgr::ptr()->init())
    // {
    //     LOG("failed\n");
    //     error_code = 13;
    //     return false;
    // }
    // LOG("done\n");

    LOG("initializing NetworkMgr... ");
    if (!NetworkMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 10;
        return false;
    }
    LOG("done\n");

    LOG("initializing ModelMgr... ");
    if (!ModelMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 10;
        return false;
    }
    LOG("done\n");

    LOG("initializing LODGroupMgr... ");
    if (!LODGroupMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 20;
        return false;
    }
    LOG("done\n");

    // =============================
    LOG("initializing SceneMgr... ");
    if (!SceneMgr::ptr()->init())
    {
        LOG("failed\n");
        error_code = 9;
        return false;
    }
    LOG("done\n");

    if (!SceneMgr::ptr()->load_scene("../res/scenes/start.scn"))
    {
        LOG("failed to load scene\n");
        error_code = 16;
        return false;
    }

    struct TransformComponent
    {
        glm::mat4 Transform;
        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::mat4& transform) : Transform(transform) {}
        operator glm::mat4&() { return Transform; }
        operator const glm::mat4&() const { return Transform; }
    };

    struct MeshComponent
    {
        int mesh_id;
        MeshComponent() = default;
        MeshComponent(const MeshComponent&) = default;

        MeshComponent(const int id) : mesh_id(id) {}
        // operator int&() { return mesh_id; }
        // operator const int&() const { return mesh_id; }

    };

    TransformComponent transform;
    MeshComponent mc;

    DoMath(transform);

    entt::entity entity = m_Registry.create();
    auto transf = m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
    auto x = m_Registry.emplace<MeshComponent>(entity, mc);
    // m_Registry.remove<TransformComponent>(entity);
    // m_Registry.clear();
    // if(m_Registry.has<TransformComponent>(entity))
    {
        TransformComponent& tr = m_Registry.get<TransformComponent>(entity);
    }

    auto view = m_Registry.view<TransformComponent>();
    for (auto entity : view)
    {
        TransformComponent& tr = view.get<TransformComponent>(entity);
    }

    auto v = m_Registry.view<TransformComponent, MeshComponent>();
    for (auto entity: v)
    {
        auto [ttt, mmm] = v.get<TransformComponent, MeshComponent>(entity);
        LOG("Entity: %d, transform[0][0]: %f, mesh_id: %d\n", (int) entity, ttt.Transform[0][0], mmm.mesh_id);
    }

    return true;
}

bool Application::main_loop()
{
    SDL_Event event;
    quit = false;

    Uint32 current_tick = SDL_GetTicks();
    Uint32 prev_tick = current_tick;
    float time_delta = 0.0f;

    Uint32 frameStart;
    int frameTime;

    while (!quit)
    {
        frameStart = SDL_GetTicks();

        while (SDL_PollEvent(&event) != 0)
        {
            if (event.type == SDL_QUIT)
            {
                quit = true;
            }
            if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_RESIZED)
                {
                    int new_w = event.window.data1;
                    int new_h = event.window.data2;
                    RenderMgr::ptr()->resize_buffers(new_w, new_h, false);
                }
            }
            if (event.type == SDL_MOUSEMOTION)
            {
                InputMgr::ptr()->mouse_motion(&(event.motion));
            }

            ImGui_ImplSDL2_ProcessEvent(&event);
        }

        ModelMgr::ptr()->update_animation_timers(time_delta);
        InputMgr::ptr()->process_input(time_delta);
        ParticleSystemMgr::ptr()->update(time_delta);
        // PhysicsMgr::ptr()->update(time_delta);

        // VideoMgr::ptr()->update(time_delta);

        SceneMgr::ptr()->update(time_delta);
        RenderMgr::ptr()->render_frame(time_delta);

        prev_tick = current_tick;
        current_tick = SDL_GetTicks();
        time_delta = current_tick - prev_tick;

        // const int fixed_FPS = 500;
        // const int fixed_FPS = 60;
        const int fixed_FPS = 30;
        // const int fixed_FPS = 24;
        const int frameDelay = 1000 / fixed_FPS;

        frameTime = SDL_GetTicks() - frameStart;

        if (frameDelay > frameTime)
        {
            SDL_Delay(frameDelay - frameTime);
        }
    }
    // SDL_Quit();
    return true;
}

void Application::exit() { quit = true; }

int Application::get_error_code() { return error_code; }

void Application::release()
{
    InputMgr::ptr()->release();
    FileSystemMgr::ptr()->release();
    SceneMgr::ptr()->release();
    // VideoMgr::ptr()->release();
    AudioMgr::ptr()->release();
    TextureMgr::ptr()->release();
    ParticleSystemMgr::ptr()->release();
    // PhysicsMgr::ptr()->release();
    FontMgr::ptr()->release();
    MaterialMgr::ptr()->release();
    ModelMgr::ptr()->release();
    LODGroupMgr::ptr()->release();
    ShaderMgr::ptr()->release();
    NetworkMgr::ptr()->release();
    RenderMgr::ptr()->release();
    LOG("Release finished\n");
    SDL_Quit();
}