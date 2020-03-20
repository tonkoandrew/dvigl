#include <dvigl/application.h>
#include <dvigl/core.h>

#include <dvigl/render_manager.h>

extern "C" int dvigl_init() {
  Application *app = Application::ptr();
  if (app->init()) {
    return 0;
  }
  return app->get_error_code();
}

extern "C" int dvigl_main() {
  Application *app = Application::ptr();
  if (app->main_loop()) {
    app->release();
    return 0;
  }
  return app->get_error_code();
}

extern "C" Uint32 dvigl_get_window_id() {
  SDL_Window *w = RenderMgr::ptr()->get_main_window();
  return SDL_GetWindowID(w);
}