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
    int visualize_normals = 0;

private:
    void lighting_pass(float time_delta);
    void geometry_pass(float time_delta, float aspect);
    void forward_pass(float aspect);
    void render_quad();
    void render_cube();

    SDL_Window* main_window;
    SDL_GLContext gl_context;
    GLuint gBuffer;
    GLuint gPosition, gNormal, gAlbedoSpec;

    GLuint quadVAO = 0;
    GLuint quadVBO;

    GLuint cubeVAO = 0;
    GLuint cubeVBO = 0;

    GLuint rboDepth;

    const GLuint NR_LIGHTS = 20;
    std::vector<glm::vec3> lightPositions;
    std::vector<glm::vec3> lightColors;
};
