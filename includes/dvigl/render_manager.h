#pragma once
#include <dvigl/core.h>

class RenderMgr : public Singleton<RenderMgr>
{
public:
    bool init();
    void render_frame(float time_delta);
    SDL_Window* get_main_window();
    void release();
    void resize_buffers(int new_w, int new_h);

private:
    void lighting_pass(float time_delta);
    void geometry_pass(float time_delta, float aspect);
    void render_quad();

    SDL_Window* main_window;
    SDL_GLContext gl_context;
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;

    GLuint quadVAO = 0;
    GLuint quadVBO;
    GLuint rboDepth;
};
