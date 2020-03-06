#include <dvigl/render_manager.h>
#include <dvigl/application.h>

#include <dvigl/shader.h>
#include <dvigl/shader_manager.h>

#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>

#include <dvigl/camera_node.h>
#include <dvigl/scene.h>
#include <dvigl/scene_manager.h>

RenderMgr gRenderMgr;

bool RenderMgr::init()
{
    SDL_GL_SetSwapInterval(0);  // set VSync
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);  // Couldn't find
    // matching GLX visual on Intel video

    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
// | SDL_WINDOW_ALLOW_HIGHDPI
// | SDL_WINDOW_MOUSE_FOCUS
// | SDL_WINDOW_INPUT_GRABBED
// | SDL_WINDOW_FULLSCREEN
#ifdef __PLATFORM_ANDROID__
                | SDL_WINDOW_BORDERLESS | SDL_WINDOW_FULLSCREEN_DESKTOP
#else
// | SDL_WINDOW_RESIZABLE
#endif
        ;

    std::string window_title = "Window title";

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
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

    // SDL_Surface * icon = IMG_Load("res/icons/icon.png");
    // SDL_SetWindowIcon(main_window, icon);

    gl_context = SDL_GL_CreateContext(main_window);

    if (!gl_context)
    {
        LOG("SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        return false;
    }

    if(!gladLoadGL()) {
        LOG("Something went wrong!\n");
        return false;
    }
    // LOG("OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);

    SDL_GL_MakeCurrent(main_window, gl_context);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_STENCIL_TEST);

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
#ifdef __PLATFORM_ANDROID__
    glClearColor(0.3f, 0.35f, 0.25f, 1.0f);
#endif

#ifndef __PLATFORM_ANDROID__
    // glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFrontFace(GL_CCW);
    // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
    return true;
}

void RenderMgr::render_frame(float time_delta)
{
    int w, h;
    SDL_GL_GetDrawableSize(main_window, &w, &h);

    // float aspect = (float)w / (float)(h>1 ? h:1);

    glViewport(0, 0, w, h);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    Shader * s = ShaderMgr::ptr()->get_shader("simple");
    s->bind();

    float aspect = (float)w / (float)(h > 1 ? h : 1);
    glm::mat4 mvp;
    glm::mat4 model_m;
    glm::mat4 proj_m = glm::perspective(45.0f, aspect, 10.0f, 1000.0f);

    Scene * current_scene = SceneMgr::ptr()->get_current_scene();
    CameraNode * camera = current_scene->get_current_camera();

    glm::mat4 view_m = camera->get_view_matrix();

    // yodacamera->move_back(300.0f);
    // yodacamera->move_up(40.0f);
    // yodacamera->yaw(-0.64f);

    glm::mat4 view_proj_m = proj_m * view_m;

    ModelNode * m = ModelMgr::ptr()->get_model("yoda");
    // m->roll(0.0005f* time_delta);
    model_m = m->get_model_matrix();
    mvp = view_proj_m * model_m;
    s->uniformMatrix4("mvp", mvp);
    s->uniform1i("is_pressed", 1);
    s->uniform1i("is_hover", 1);
    TextureMgr::ptr()->get_texture("yoda")->bind();

    s->uniform1f("ui_alpha", 1.0f);
    m->draw();


    SDL_GL_SwapWindow(main_window);
}

SDL_Window * RenderMgr::get_main_window() { return main_window; }
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
