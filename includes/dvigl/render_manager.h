#pragma once
#include <dvigl/core.h>

class RenderMgr : public Singleton<RenderMgr> {
public:
  bool init();
  void render_frame(float time_delta);
  SDL_Window *get_main_window();
  void release();

private:
  SDL_Window *main_window;
  SDL_GLContext gl_context;
};
