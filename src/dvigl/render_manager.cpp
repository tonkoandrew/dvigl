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

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);
    // int w = 320;
    // int h = 240;

    int w = 1024;
    int h = 768;

    // int w = 1920;
    // int h = 1080;

    main_window = SDL_CreateWindow(window_title.c_str(), 100, 100, w, h, flags);

    if (!main_window)
    {
        LOG("SDL_CreateWindow failed: %s\n", SDL_GetError());
        return false;
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

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_CULL_FACE);
    // glDisable(GL_CULL_FACE);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LEQUAL);

    glClearColor(0.01f, 0.01f, 0.01f, 1.0f);

    // glClearColor(0.05f, 0.5f, 0.05f, 1.0f);
    // #ifdef __PLATFORM_ANDROID__
    //   glClearColor(0.3f, 0.35f, 0.25f, 1.0f);
    // #endif

#if !defined(__PLATFORM_ANDROID__)
    // LOG("\n++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    // int max_attrs = 0;
    // glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &max_attrs);
    // LOG("%d\n", max_attrs);

    // LOG("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
    // // glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glFrontFace(GL_CCW);
// glFrontFace(GL_CW);
// glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif

    SDL_GL_GetDrawableSize(main_window, &w, &h);
    glViewport(0, 0, w, h);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SDL_GL_SwapWindow(main_window);

    resize_buffers(w, h, true);

    float a = 300.0f;
    float b = a / 2.0f;
    for (unsigned int i = 0; i < NR_LIGHTS; i++)
    {
        // calculate slightly random offsets
        float xPos = ((rand() % 100) / 100.0) * a - b;

        // float yPos = ((rand() % 100) / 100.0) * a - b;
        // yPos = (yPos / 2.0f) + 20.0f;
        float yPos = ((rand() % 100) / 100.0) * a / 3;
        float zPos = ((rand() % 100) / 100.0) * a - b;

        lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
        // also calculate random color
        float rColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float gColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        float bColor = ((rand() % 100) / 200.0f) + 0.5; // between 0.5 and 1.0
        lightColors.push_back(glm::vec3(rColor, gColor, bColor));
    }
    return true;
}

void RenderMgr::geometry_pass(float time_delta, float aspect)
{
    glDisable(GL_BLEND);

    Shader* s;
    glm::mat4 model_m;
    glm::mat4 view_m;
    glm::mat4 proj_m;
    glm::mat4 view_proj_m;
    glm::mat4 mvp;

    proj_m = glm::perspective(45.0f, aspect, 0.1f, 1000.0f);

    CameraNode* camera = SceneMgr::ptr()->get_current_scene()->get_current_camera();

    view_m = camera->get_view_matrix();

    view_proj_m = proj_m * view_m;

    s = ShaderMgr::ptr()->get_shader("simple");
    s->bind();
    s->uniform1i("albedoMap", 0);
    s->uniform1i("normalMap", 1);

    for (auto element : ModelMgr::ptr()->models)
    {
        ModelNode* m = (ModelNode*)element.second;
        model_m = m->get_model_matrix();
        mvp = view_proj_m * model_m;
        s->uniformMatrix4("model", model_m);
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
        s->uniformMatrix4("model", model_m);
        s->uniformMatrix4("mvp", mvp);
        m->draw();
    }
}

void RenderMgr::lighting_pass(float time_delta)
{
    Shader* s;
    s = ShaderMgr::ptr()->get_shader("lighting");
    s->bind();
    s->uniform1i("visualize_normals", visualize_normals);

    // send light relevant uniforms
    for (unsigned int i = 0; i < lightPositions.size(); i++)
    {
        s->uniform3f("lights[" + std::to_string(i) + "].Position", lightPositions[i]);
        s->uniform3f("lights[" + std::to_string(i) + "].Color", lightColors[i]);

        // update attenuation parameters
        const float constant = 1.0;
        const float linear = 0.7f;
        const float quadratic = 1.8f;

        s->uniform1f("lights[" + std::to_string(i) + "].Linear", linear);
        s->uniform1f("lights[" + std::to_string(i) + "].Quadratic", quadratic);
        // // then calculate radius of light volume/sphere
        // const float maxBrightness = std::fmaxf(std::fmaxf(lightColors[i].r, lightColors[i].g), lightColors[i].b);
        // float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (coeff)*maxBrightness)))
            // / (2.0f * quadratic);
        // LOG("radius = %f\n", radius);
        // s->uniform1f("lights[" + std::to_string(i) + "].Radius", radius);
    }

    CameraNode* camera = SceneMgr::ptr()->get_current_scene()->get_current_camera();
    s->uniform3f("viewPos", camera->position);

    glActiveTexture(GL_TEXTURE0);
    s->uniform1i("gPosition", 0);
    glBindTexture(GL_TEXTURE_2D, gPosition);

    glActiveTexture(GL_TEXTURE1);
    s->uniform1i("gNormal", 1);
    glBindTexture(GL_TEXTURE_2D, gNormal);

    glActiveTexture(GL_TEXTURE2);
    s->uniform1i("gAlbedoSpec", 2);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);

    render_quad();
}

void RenderMgr::forward_pass(float aspect)
{

    Shader* s;

    glm::mat4 model_m;
    glm::mat4 view_m;
    glm::mat4 proj_m;
    glm::mat4 mvp;
    glm::mat4 view_proj_m;

    proj_m = glm::perspective(45.0f, aspect, 0.1f, 1000.0f);
    CameraNode* camera = SceneMgr::ptr()->get_current_scene()->get_current_camera();
    view_m = camera->get_view_matrix();
    view_proj_m = proj_m * view_m;

    s = ShaderMgr::ptr()->get_shader("forward");
    s->bind();

    for (unsigned int i = 0; i < lightPositions.size(); i++)
    {
        model_m = glm::mat4(1.0f);
        model_m = glm::translate(model_m, lightPositions[i]);
        model_m = glm::scale(model_m, glm::vec3(0.5f));

        mvp = view_proj_m * model_m;
        s->uniformMatrix4("mvp", mvp);
        s->uniform3f("lightColor", lightColors[i]);
        render_cube();
    }
}

void RenderMgr::render_frame(float time_delta)
{
    int w, h;
    glGetError();
    SDL_GL_MakeCurrent(main_window, gl_context);
    SDL_GL_GetDrawableSize(main_window, &w, &h);
    float aspect = (float)w / (float)(h > 1 ? h : 1);

    glViewport(0, 0, w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    geometry_pass(time_delta, aspect);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    lighting_pass(time_delta);

    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    // ----------------------------------------------------------------------------------
    glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
    // blit to default framebuffer. Note that this may or may not work as the internal formats of both the FBO and
    // default framebuffer have to match.
    // the internal formats are implementation defined. This works on all of my systems, but if it doesn't on yours
    // you'll likely have to write to the
    // depth buffer in another shader stage (or somehow see to match the default framebuffer's internal format with the
    // FBO's internal format).
    glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    forward_pass(aspect);

    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("GL ERRORS HERE ================\n");
        LOG("%d \n", err);
        LOG("================\n");
    }
    SDL_GL_SwapWindow(main_window);
}

void RenderMgr::render_quad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,
            1.0f,
            0.0f,
            0.0f,
            1.0f,
            -1.0f,
            -1.0f,
            0.0f,
            0.0f,
            0.0f,
            1.0f,
            1.0f,
            0.0f,
            1.0f,
            1.0f,
            1.0f,
            -1.0f,
            0.0f,
            1.0f,
            0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void RenderMgr::render_cube()
{
    // initialize (if necessary)
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f, // top-right
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, // top-left
            // front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, // top-right
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, // top-left
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom-left
            // left face
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            -1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-right
            // right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, // bottom-right
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, // top-left
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom-left
            // bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f, // top-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, // bottom-left
            -1.0f, -1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, // bottom-right
            -1.0f, -1.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, // top-right
            // top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, // top-right
            1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom-right
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, // top-left
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f // bottom-left
        };
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        // fill buffer
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        // link vertex attributes
        glBindVertexArray(cubeVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
    // render Cube
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void RenderMgr::resize_buffers(int w, int h, bool initialize)
{
    if (initialize)
        glGenFramebuffers(1, &gBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // - position color buffer

    if (initialize)
        glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // - normal color buffer
    if (initialize)
        glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - color + specular color buffer
    if (initialize)
        glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering
    GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("Framebuffer not complete!\n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
