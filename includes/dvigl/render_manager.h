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
    int visualize_albedo = 0;
    int visualize_normals = 0;
    int visualize_metallic = 0;
    int visualize_roughness = 0;
    int visualize_ao = 0;
    int visualize_world_position = 0;

    float z_near;
    float z_far;
    float fov;
private:
    void geometry_pass(float time_delta, float aspect);
    void deferred_pass(float time_delta, float aspect);
    void forward_pass(float aspect);
    void render_quad();
    void render_cube();

    SDL_Window* main_window;
    SDL_GLContext gl_context;
    GLuint gBuffer;
    GLuint gAlbedo, gNormal, gMaterialInfo, gPos;

    GLuint quadVAO = 0;
    GLuint quadVBO;

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;

    GLuint rboDepth;

};
