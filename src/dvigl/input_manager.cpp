#include <dvigl/application.h>
#include <dvigl/input_manager.h>

#include <dvigl/camera_node.h>
#include <dvigl/scene.h>
#include <dvigl/scene_manager.h>

InputMgr gInputMgr;

bool InputMgr::init() { return true; }

void InputMgr::process_input(float time_delta)
{
    keystates = SDL_GetKeyboardState(NULL);

    if (keystates[SDL_SCANCODE_AC_HOME] || keystates[SDL_SCANCODE_Q]) {
        Application::ptr()->exit();
    }

    CameraNode* cam = SceneMgr::ptr()->get_current_scene()->get_current_camera();

    static float angle = 0;
    static float dist = 0;
    if (keystates[SDL_SCANCODE_W]) {
        cam->move_forward(0.1f * time_delta);
        dist -= 0.01 * time_delta;
    }

    if (keystates[SDL_SCANCODE_S]) {
        cam->move_back(0.1f * time_delta);

        dist += 0.01 * time_delta;
    }

    if (keystates[SDL_SCANCODE_Z]) {
        cam->move_left(0.1f * time_delta);
    }

    if (keystates[SDL_SCANCODE_C]) {
        cam->move_right(0.1f * time_delta);
    }

    if (keystates[SDL_SCANCODE_F]) {
        cam->move_down(0.1f * time_delta);
    }

    if (keystates[SDL_SCANCODE_R]) {
        cam->move_up(0.1f * time_delta);
    }

    if (keystates[SDL_SCANCODE_A]) {
        cam->yaw(0.001f * time_delta);
        angle += 0.1 * time_delta;
    }

    if (keystates[SDL_SCANCODE_D]) {
        cam->yaw(-0.001f * time_delta);
        angle -= 0.1 * time_delta;
    }

    Mix_SetPosition(MIX_CHANNEL_POST, angle, dist);
}

void InputMgr::release() {}