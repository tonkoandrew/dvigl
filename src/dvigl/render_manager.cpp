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

#include <dvigl/dir_light_node.h>
#include <dvigl/point_light_node.h>
#include <dvigl/spot_light_node.h>

#include <dvigl/frustum.h>

#include <dvigl/lod_group.h>
#include <dvigl/lod_group_manager.h>

RenderMgr gRenderMgr;

bool RenderMgr::init()
{

    // if (SDL_SetRelativeMouseMode(SDL_TRUE))
    // {
    //     LOG("SDL_SetRelativeMouseMode failed: %s\n", SDL_GetError());
    //     return false;
    // }
    LOG("\n");

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    // SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 0);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 0);

#if defined(__PLATFORM_APPLE__)
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
#endif

    int flags = SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI
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

    // int w = 320;
    // int h = 240;

    int w = 1024;
    int h = 768;

    // int w = 1920;
    // int h = 1080;

    main_window = SDL_CreateWindow(window_title.c_str(), 2000, 100, w, h, flags);

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

    const char* sdl_error = SDL_GetError();
    if (strlen(sdl_error) != 0)
    {
        LOG("*************************\nSDL ERRORS: %s\n", sdl_error);
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); //(void)io;
    io.Fonts->AddFontFromFileTTF("../res/fonts/SourceCodePro-Regular.ttf", 16);
    // io.Fonts->GetTexDataAsRGBA32()

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

    ImGui_ImplSDL2_InitForOpenGL(main_window, gl_context);
    ImGui_ImplOpenGL3_Init();

    // set VSync
    // if (SDL_GL_SetSwapInterval(0)) {
    // if (SDL_GL_SetSwapInterval(1)) {
    if (SDL_GL_SetSwapInterval(-1))
    {
        LOG("SDL_GL_SetSwapInterval failed: %s\n", SDL_GetError());
    }

    // if (GLAD_GL_ARB_pipeline_statistics_query)
    // {
    //     LOG("***********************************************************\n");
    //     LOG("***********************************************************\n");
    //     LOG("***********************************************************\n");
    // }
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

    // glEnable(GL_MULTISAMPLE);

    glEnable(GL_STENCIL_TEST);
    glDisable(GL_POLYGON_SMOOTH);
    glEnable(GL_LINE_SMOOTH);

    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);
    // glDisable(GL_CULL_FACE);

    glDisable(GL_BLEND);
    // glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0, 1.0);

    // glClearColor(0.01f, 0.1f, 0.01f, 1.0f);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    // glClearColor(0.05f, 0.5f, 0.05f, 1.0f);
    // #ifdef __PLATFORM_ANDROID__
    //   glClearColor(0.3f, 0.35f, 0.25f, 1.0f);
    // #endif

    int max_tex_size;
    glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &max_tex_size);
    LOG("====================================================\n");
    LOG("Max texture buffer size is %d\n", max_tex_size);
    LOG("====================================================\n");

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

    fov = 45.0f;
    z_near = 0.1f;
    z_far = 5000.0f;

    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("GL Renderer init ERRORS HERE ================\n");
        LOG("%d \n", err);
        LOG("================\n");
    }

    resize_buffers(w, h, true);

    return true;
}

void RenderMgr::geometry_pass(float time_delta, float aspect)
{
    if (visualize_wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glDisable(GL_BLEND);

    float dt = time_delta * 1000.0f;

    Shader* s;
    glm::mat4 model_m;
    glm::mat4 view_m;
    glm::mat4 proj_m;

    glm::mat4 view_proj_m;
    glm::mat4 mvp;
    glm::mat4 prev_mvp;

    Frustum f;

    proj_m = glm::perspective(fov, aspect, z_near, z_far);

    CameraNode* camera = SceneMgr::ptr()->get_current_scene()->get_current_camera();

    view_m = camera->get_view_matrix();

    view_proj_m = proj_m * view_m;

    f.update(view_proj_m);

    s = ShaderMgr::ptr()->get_shader("static_geometry");
    s->bind();
    s->uniform1i("material.texture_albedo", 0);
    s->uniform1i("material.texture_normal", 1);
    s->uniform1i("material.texture_metallic", 2);
    s->uniform1i("material.texture_roughness", 3);
    s->uniform1i("material.texture_ao", 4);

    // s->uniform1f("time_delta", dt);

    int rendered_objects = 0;

    for (auto element : LODGroupMgr::ptr()->lod_groups)
    {
        LODGroup* grp = (LODGroup*)element.second;
        float bounding_radius = grp->models[0]->get_bounding_sphere_radius();
        if (!f.sphere_test(grp->position, bounding_radius))
        {
            continue;
        }
        rendered_objects += 1;

        float dist = glm::distance(grp->position, camera->position);

        grp->update(dist);
    }

    for (auto element : ModelMgr::ptr()->models)
    {
        ModelNode* m = (ModelNode*)element.second;
        if (!m->visible)
        {
            continue;
        }

        model_m = m->get_model_matrix();
        float bounding_radius = m->get_bounding_sphere_radius();
        if (!f.sphere_test(m->position, bounding_radius))
        {
            continue;
        }
        rendered_objects += 1;

        mvp = view_proj_m * model_m;
        // glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(mvp)));
        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model_m)));

        s->uniformMatrix3("normalMatrix", normalMatrix);
        s->uniformMatrix4("model", model_m);
        s->uniformMatrix4("mvp", mvp);
        s->uniformMatrix4("prev_mvp", m->prev_mvp);

        m->draw();
        m->prev_mvp = mvp;
    }

    s = ShaderMgr::ptr()->get_shader("skinned_geometry");
    s->bind();
    s->uniform1i("material.texture_albedo", 0);
    s->uniform1i("material.texture_normal", 1);
    s->uniform1i("material.texture_metallic", 2);
    s->uniform1i("material.texture_roughness", 3);
    s->uniform1i("material.texture_ao", 4);

    // s->uniform1f("time_delta", dt);

    for (auto element : ModelMgr::ptr()->skinned_models)
    {
        SkinnedModelNode* m = (SkinnedModelNode*)element.second;
        model_m = m->get_model_matrix();

        float bounding_radius = m->get_bounding_sphere_radius();
        if (!f.sphere_test(m->position, bounding_radius))
        {
            continue;
        }
        rendered_objects += 1;

        mvp = view_proj_m * model_m;

        glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(model_m)));

        s->uniformMatrix4("model", model_m);
        s->uniformMatrix4("mvp", mvp);
        s->uniformMatrix4("prev_mvp", m->prev_mvp);
        s->uniformMatrix3("normalMatrix", normalMatrix);
        m->draw();

        m->prev_mvp = mvp;
    }

    static int last_count = 0;

    if (last_count != rendered_objects)
    {
        last_count = rendered_objects;
        LOG("%d objects rendered\n", rendered_objects);
    }

    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("geometry pass GL ERRORS HERE ================\n");
        LOG("%d \n", err);
        LOG("================\n");
    }

    prev_proj_m = glm::mat4(proj_m);

    if (visualize_wireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void RenderMgr::deferred_pass(float time_delta, float aspect)
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Shader* s;
    s = ShaderMgr::ptr()->get_shader("deferred");
    s->bind();

    glm::mat4 model_m;
    glm::mat4 view_m;
    glm::mat4 proj_m;
    glm::mat4 view_proj_m;

    proj_m = glm::perspective(fov, aspect, z_near, z_far);
    CameraNode* camera = SceneMgr::ptr()->get_current_scene()->get_current_camera();
    view_m = camera->get_view_matrix();
    view_proj_m = proj_m * view_m;

    float velocity_scale = 20.0f;
    // float velocity_scale = 0.001f + (1.0f * time_delta);
    // LOG("%f\n", velocity_scale);
    s->uniform1f("uVelocityScale", velocity_scale);

    s->uniform3f("viewPos", camera->get_position());

    s->uniform1i("visualize_albedo", visualize_albedo);
    s->uniform1i("visualize_normals", visualize_normals);
    s->uniform1i("visualize_metallic", visualize_metallic);
    s->uniform1i("visualize_roughness", visualize_roughness);
    s->uniform1i("visualize_ao", visualize_ao);
    s->uniform1i("visualize_world_position", visualize_world_position);
    s->uniform1i("visualize_velocity", visualize_velocity);
    s->uniform1i("visualize_wireframe", visualize_wireframe);

    s->uniform3i("numDirPointSpotLights", glm::ivec3((int)SceneMgr::ptr()->get_current_scene()->dir_lights.size(),
                                              (int)SceneMgr::ptr()->get_current_scene()->point_lights.size(),
                                              (int)SceneMgr::ptr()->get_current_scene()->spot_lights.size()));

    // send light relevant uniforms
    int i = 0;
    for (auto element : SceneMgr::ptr()->get_current_scene()->dir_lights)
    {
        DirLightNode* light = (DirLightNode*)element.second;
        s->uniform3f("dirLights[" + std::to_string(i) + "].direction", light->get_direction());
        s->uniform3f("dirLights[" + std::to_string(i) + "].lightColour", light->color);
        s->uniform1f("dirLights[" + std::to_string(i) + "].intensity", light->intensity);
        i += 1;
    }

    i = 0;
    for (auto element : SceneMgr::ptr()->get_current_scene()->point_lights)
    {
        PointLightNode* light = (PointLightNode*)element.second;
        s->uniform3f("pointLights[" + std::to_string(i) + "].position", light->position);
        s->uniform1f("pointLights[" + std::to_string(i) + "].intensity", light->intensity);
        s->uniform3f("pointLights[" + std::to_string(i) + "].lightColour", light->color);
        s->uniform1f("pointLights[" + std::to_string(i) + "].attenuationRadius", light->attenuationRadius);
        i += 1;
    }

    i = 0;
    for (auto element : SceneMgr::ptr()->get_current_scene()->spot_lights)
    {
        SpotLightNode* light = (SpotLightNode*)element.second;
        s->uniform3f("spotLights[" + std::to_string(i) + "].position", light->position);
        s->uniform3f("spotLights[" + std::to_string(i) + "].direction", light->get_direction());
        s->uniform1f("spotLights[" + std::to_string(i) + "].intensity", light->intensity);
        s->uniform3f("spotLights[" + std::to_string(i) + "].lightColour", light->color);
        s->uniform1f("spotLights[" + std::to_string(i) + "].attenuationRadius", light->attenuationRadius);
        s->uniform1f("spotLights[" + std::to_string(i) + "].cutOff", light->cutOff);
        s->uniform1f("spotLights[" + std::to_string(i) + "].outerCutOff", light->outerCutOff);
        i += 1;
    }

    glActiveTexture(GL_TEXTURE0);
    s->uniform1i("materialInfoTexture", 0);
    glBindTexture(GL_TEXTURE_2D, gMaterialInfo);

    glActiveTexture(GL_TEXTURE1);
    s->uniform1i("normalTexture", 1);
    glBindTexture(GL_TEXTURE_2D, gNormal);

    glActiveTexture(GL_TEXTURE2);
    s->uniform1i("albedoTexture", 2);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);

    glActiveTexture(GL_TEXTURE3);
    s->uniform1i("worldPosTexture", 3);
    glBindTexture(GL_TEXTURE_2D, gPos);

    glActiveTexture(GL_TEXTURE4);
    s->uniform1i("velocityTexture", 4);
    glBindTexture(GL_TEXTURE_2D, gVelocity);

    render_quad();
    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("deferred pass GL ERRORS HERE ================\n");
        LOG("%d \n", err);
        LOG("================\n");
    }
}

void RenderMgr::forward_pass(float aspect)
{

    Shader* s;

    glm::mat4 model_m;
    glm::mat4 view_m;
    glm::mat4 proj_m;
    glm::mat4 mvp;
    glm::mat4 view_proj_m;

    proj_m = glm::perspective(fov, aspect, z_near, z_far);
    CameraNode* camera = SceneMgr::ptr()->get_current_scene()->get_current_camera();
    view_m = camera->get_view_matrix();
    view_proj_m = proj_m * view_m;

    s = ShaderMgr::ptr()->get_shader("forward");
    s->bind();

    for (auto element : SceneMgr::ptr()->get_current_scene()->point_lights)
    {
        PointLightNode* light = (PointLightNode*)element.second;
        model_m = light->get_model_matrix();
        mvp = view_proj_m * model_m;
        s->uniformMatrix4("mvp", mvp);
        s->uniform3f("lightColor", light->color);
        render_cube();
    }

    for (auto element : SceneMgr::ptr()->get_current_scene()->spot_lights)
    {
        SpotLightNode* light = (SpotLightNode*)element.second;
        model_m = light->get_model_matrix();
        mvp = view_proj_m * model_m;
        s->uniformMatrix4("mvp", mvp);
        s->uniform3f("lightColor", light->color);
        render_cube();
    }

    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("forward pass GL ERRORS HERE ================\n");
        LOG("%d \n", err);
        LOG("================\n");
    }
}

void RenderMgr::shadow_pass() {}

void RenderMgr::render_frame(float time_delta)
{
    int w, h;
    SDL_GL_MakeCurrent(main_window, gl_context);
    SDL_GL_GetDrawableSize(main_window, &w, &h);
    float aspect = (float)w / (float)(h > 1 ? h : 1);

    glViewport(0, 0, w, h);

    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // GLuint    query;
    // GLuint64 elapsedTime;
    // glGenQueries ( 1, &query );
    // // glBeginQuery ( GL_VERTICES_SUBMITTED_ARB, query );
    // glBeginQuery ( GL_PRIMITIVES_SUBMITTED_ARB, query );

    geometry_pass(time_delta, aspect);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    deferred_pass(time_delta, aspect);

    // 2.5. copy content of geometry's depth buffer to default framebuffer's depth buffer
    // ----------------------------------------------------------------------------------
    // glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
    // glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    // glBlitFramebuffer(0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
    glBlitNamedFramebuffer(gBuffer, 0, 0, 0, w, h, 0, 0, w, h, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    forward_pass(aspect);

    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("GL ERRORS HERE ================\n");
        LOG("%d \n", err);
        LOG("================\n");
    }

    // Start the Dear ImGui frame
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(main_window);
    ImGui::NewFrame();
    // static bool show_demo_window = true;
    // if (show_demo_window)
    //     ImGui::ShowDemoWindow(&show_demo_window);

    ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(100.0f, 200.0f), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings", NULL, 0);
    ImGui::Text("Animation speed:");
    static float animation_speed = 0.05f;
    ImGui::SliderFloat(" ", &animation_speed, 0.0f, 2.0f);
    ImGui::Separator();
    ImGui::Text("Antialiasing technique:");
    static bool use_fxaa = false;
    ImGui::Checkbox("FXAA", &use_fxaa);
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // glEndQuery ( GL_PRIMITIVES_SUBMITTED_ARB );
    // glGetQueryObjectui64v ( query, GL_QUERY_RESULT, &elapsedTime );
    // LOG("GL_PRIMITIVES_SUBMITTED_ARB = %d\n", elapsedTime);
    // // LOG("GL_VERTICES_SUBMITTED_ARB = %d\n", elapsedTime);

    SDL_GL_SwapWindow(main_window);

    const char* sdl_error = SDL_GetError();
    if (strlen(sdl_error) != 0)
    {
        LOG("*************************\nSDL ERRORS: %s\n", sdl_error);
    }
}

void RenderMgr::render_quad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f,
            0.0f, 1.0f, 0.0f,
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

    if (initialize)
    {
        glGenTextures(1, &gAlbedo);
    }
    else
    {
        glDeleteTextures(1, &gAlbedo);
        glGenTextures(1, &gAlbedo);
    }

    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gAlbedo, 0);

    if (initialize)
    {
        glGenTextures(1, &gNormal);
    }
    else
    {
        glDeleteTextures(1, &gNormal);
        glGenTextures(1, &gNormal);
    }
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    if (initialize)
    {
        glGenTextures(1, &gMaterialInfo);
    }
    else
    {
        glDeleteTextures(1, &gMaterialInfo);
        glGenTextures(1, &gMaterialInfo);
    }
    glBindTexture(GL_TEXTURE_2D, gMaterialInfo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gMaterialInfo, 0);

    if (initialize)
    {
        glGenTextures(1, &gPos);
    }
    else
    {
        glDeleteTextures(1, &gPos);
        glGenTextures(1, &gPos);
    }
    glBindTexture(GL_TEXTURE_2D, gPos);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, w, h, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gPos, 0);

    if (initialize)
    {
        glGenTextures(1, &gVelocity);
    }
    else
    {
        glDeleteTextures(1, &gVelocity);
        glGenTextures(1, &gVelocity);
    }
    glBindTexture(GL_TEXTURE_2D, gVelocity);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, w, h, 0, GL_RG, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT4, GL_TEXTURE_2D, gVelocity, 0);

    GLuint attachments[5] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3,
        GL_COLOR_ATTACHMENT4 };
    glDrawBuffers(5, attachments);
    // glDrawBuffers(3, attachments);
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        LOG("Framebuffer not complete!\n");
    }

    GLuint err = glGetError();
    if (err != 0)
    {
        LOG("G-Buffer setup error ================\n");
        LOG("%d \n", err);
        LOG("================\n");
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
