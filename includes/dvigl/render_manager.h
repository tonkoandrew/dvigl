#pragma once
#include <dvigl/core.h>

class RenderMgr : public Singleton<RenderMgr>
{
public:
    bool init();
    void render_frame(float time_delta);
    SDL_Window* get_main_window();
    void release();
    void resize_buffers(int new_w, int new_h, bool initialize);
    bool visualize_albedo = false;
    bool visualize_normals = false;
    bool visualize_metallic = false;
    bool visualize_roughness = false;
    bool visualize_ao = false;
    bool visualize_world_position = false;
    bool visualize_velocity = false;
    bool visualize_wireframe = false;

    float z_near;
    float z_far;
    float fov;

private:
    void shadow_pass();
    void geometry_pass(float time_delta, float aspect);
    void deferred_pass(float time_delta, float aspect);
    void forward_pass(float aspect);
    void render_quad();
    void render_cube();

    SDL_Window* main_window;
    SDL_GLContext gl_context;
    GLuint gBuffer;
    GLuint gAlbedo, gNormal, gMaterialInfo, gPos, gVelocity;

    GLuint quadVAO = 0;
    GLuint quadVBO;

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;

    GLuint rboDepth;
};
