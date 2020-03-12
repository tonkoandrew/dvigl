#include <dvigl/application.h>
#include <dvigl/input_manager.h>

InputMgr gInputMgr;

bool InputMgr::init() { return true; }

void InputMgr::process_input(float time_delta) {
  keystates = SDL_GetKeyboardState(NULL);

  if (keystates[SDL_SCANCODE_AC_HOME] || keystates[SDL_SCANCODE_Q]) {
    Application::ptr()->exit();
  }
}

void InputMgr::release() {}