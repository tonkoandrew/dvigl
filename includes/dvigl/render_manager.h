#pragma once
#include <dvigl/core.h>

enum VisualizationType
{
    VIS_NONE,
    VIS_ALBEDO,
    VIS_NORMALS,
    VIS_METALLNESS,
    VIS_ROUGHNESS,
    VIS_AMBIENT_OCCLUSION,
    VIS_WORLD_POSITIONS,
    VIS_VELOCITY,
    VIS_WIREFRAME
};

class RenderMgr : public Singleton<RenderMgr>
{
public:
    bool init();
    void render_frame(float time_delta);
    SDL_Window* get_main_window();
    void release();
    void resize_buffers(int new_w, int new_h, bool initialize);
    VisualizationType vis_type = VIS_NONE;

    float z_near;
    float z_far;
    float fov;

private:
    void shadow_pass();
    void geometry_pass(float time_delta, float aspect);
    void deferred_pass(float time_delta, float aspect);
    void forward_pass(float aspect);
    void gui_pass();
    void render_quad();
    void render_cube();

    SDL_Window* main_window;
    SDL_GLContext gl_context;
    GLuint gBuffer; // framebuffer for deferred rendering pass
    GLuint gAlbedo, gNormal, gMaterialInfo, gPos, gVelocity; // gBuffer textures

    GLuint shadowMapFBO; // Shadow map framebuffer
    GLuint shadowMap;
    const GLuint SHADOW_WIDTH = 2048;
    const GLuint SHADOW_HEIGHT = 2048;

    GLuint quadVAO = 0;
    GLuint quadVBO;

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;

    GLuint rboDepth;

    bool reload_shaders = false;

    float sun_pos_x = 255.0f;
    float sun_pos_y = 378.0f;
    float sun_pos_z = -5.0f;
    float shadow_near_plane = 100.0f;
    float shadow_far_plane = 1000.0f;
    float shadow_frustum_size = 250.0f;

    float shadowBias = 0.0005f;
    glm::mat4 lightSpaceMatrix;
};
