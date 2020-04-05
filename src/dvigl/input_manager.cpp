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

    if (keystates[SDL_SCANCODE_AC_HOME] || keystates[SDL_SCANCODE_Q])
    {
        Application::ptr()->exit();
    }

    CameraNode* cam = SceneMgr::ptr()->get_current_scene()->get_current_camera();

float movement_speed = 0.5f;
float rotation_speed = 0.002f;

    if (keystates[SDL_SCANCODE_W])
    {
        cam->move_forward(movement_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_S])
    {
        cam->move_back(movement_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_Z])
    {
        cam->move_left(movement_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_C])
    {
        cam->move_right(movement_speed* time_delta);
    }

    if (keystates[SDL_SCANCODE_F])
    {
        cam->move_down(movement_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_R])
    {
        cam->move_up(movement_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_A])
    {
        cam->yaw(rotation_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_D])
    {
        cam->yaw(-rotation_speed * time_delta);
    }
}

void InputMgr::release() {}