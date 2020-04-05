#include <dvigl/application.h>
#include <dvigl/render_manager.h>

#include <dvigl/shader.h>
#include <dvigl/shader_manager.h>

#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>
#include <dvigl/skinned_model_node.h>

#include <dvigl/camera_node.h>
#include <dvigl/scene.h>
#include <dvigl/scene_manager.h>

RenderMgr gRenderMgr;

bool RenderMgr::init()
{
    LOG("\n");
    SDL_GL_SetSwapInterval(0); // set VSync
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);

    int flags = SDL_WINDOW_SHOWN
        | SDL_WINDOW_OPENGL
        // | SDL_WINDOW_ALLOW_HIGHDPI
        // | SDL_WINDOW_MOUSE_FOCUS
        // | SDL_WINDOW_INPUT_GRABBED
        // | SDL_WINDOW_FULLSCREEN
        // | SDL_WINDOW_BORDERLESS
        | SDL_WINDOW_RESIZABLE
#ifdef __PLATFORM_ANDROID__
        | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP
#else
// | SDL_WINDOW_RESIZABLE
#endif
        ;

    std::string window_title = "Window title";

#if defined(__PLATFORM_APPLE__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    // int w = 320;
    // int h = 240;

    int w = 1024;
    int h = 768;

    // int w = 1920;
    // int h = 1080;

    main_window = SDL_CreateWindow(window_title.c_str(), 100, 100, w, h, flags);

    if (!main_window)
    {
        SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 1);
        main_window = SDL_CreateWindow(window_title.c_str(), 100, 100, w, h, flags);
        if (!main_window)
        {
            SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
            main_window = SDL_CreateWindow(window_title.c_str(), 100, 100, w, h, flags);
            if (!main_window)
            {
                LOG("SDL_CreateWindow failed: %s\n", SDL_GetError());
                return false;
            }
        }
    }
    SDL_ClearError();

    SDL_Surface* icon = IMG_Load("../res/icons/icon.png");
    SDL_SetWindowIcon(main_window, icon);

    gl_context = SDL_GL_CreateContext(main_window);

    if (!gl_context)
    {
        LOG("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
    {
        LOG("Failed to initialize the OpenGL context.\n");
        return false;
    }

    LOG("rendering context: OpenGL %d.%d Core profile\n", GLVersion.major, GLVersion.minor);

    if (!GLAD_GL_ARB_texture_multisample)
    {
        LOG("GL_ARB_texture_multisample is not supported\n");
        return false;
    }

    if (!GLAD_GL_ARB_draw_elements_base_vertex)
    {
        LOG("GL_ARB_draw_elements_base_vertex is not supported\n");
        return false;
    }

    SDL_GL_MakeCurrent(main_window, gl_context);

    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    // glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);

    // glClearColor(0.05f, 0.5f, 0.05f, 1.0f);
    // #ifdef __PLATFORM_ANDROID__
    //   glClearColor(0.3f, 0.35f, 0.25f, 1.0f);
    // #endif

#ifndef __PLATFORM_ANDROID__
    // LOG("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    // int max_attrs = 0;
    // glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_attrs);
    // LOG("%d\n", max_attrs);

    // LOG("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    // // glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFrontFace(GL_CCW);
// glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif

    SDL_GL_GetDrawableSize(main_window, &w, &h);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SDL_GL_SwapWindow(main_window);
    return true;
}

void RenderMgr::render_frame(float time_delta)
{
    int w, h;
    Shader* s;
    glm::mat4 model_m;
    glm::mat4 view_m;
    glm::mat4 proj_m;
    glm::mat4 mvp;
    glm::mat4 view_proj_m;

    SDL_GL_MakeCurrent(main_window, gl_context);
    SDL_GL_GetDrawableSize(main_window, &w, &h);

    glViewport(0, 0, w, h);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = (float)w / (float)(h > 1 ? h : 1);

    proj_m = glm::perspective(45.0f, aspect, 1.0f, 10000.0f);

    CameraNode* camera = SceneMgr::ptr()->get_current_scene()->get_current_camera();

    view_m = camera->get_view_matrix();

    view_proj_m = proj_m * view_m;

    s = ShaderMgr::ptr()->get_shader("simple");
    s->bind();
    s->uniform1i("tex", 0);

    for (auto element : ModelMgr::ptr()->models)
    {
        ModelNode* m = (ModelNode*)element.second;
        model_m = m->get_model_matrix();
        mvp = view_proj_m * model_m;
        s->uniformMatrix4("mvp", mvp);

        m->draw();
    }

    s = ShaderMgr::ptr()->get_shader("skinned");
    s->bind();

    for (auto element : ModelMgr::ptr()->skinned_models)
    {
        SkinnedModelNode* m = (SkinnedModelNode*)element.second;
        model_m = m->get_model_matrix();
        mvp = view_proj_m * model_m;
        s->uniformMatrix4("mvp", mvp);
        m->draw();
    }

    SDL_GL_SwapWindow(main_window);

    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("GL ERRORS HERE ================\n");
        LOG("%d \n", err);
        LOG("================\n");
    }
}

SDL_Window* RenderMgr::get_main_window() { return main_window; }

void RenderMgr::release()
{
    if (gl_context)
    {
        SDL_GL_DeleteContext(gl_context);
    }

    if (main_window)
    {
        SDL_DestroyWindow(main_window);
    }
    SDL_Quit();
}
