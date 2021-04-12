#include <dvigl/application.h>
#include <dvigl/input_manager.h>

#include <dvigl/camera_node.h>
#include <dvigl/scene.h>
#include <dvigl/scene_manager.h>

#include <dvigl/model_manager.h>
#include <dvigl/skinned_model_node.h>

#include <dvigl/render_manager.h>

#include <dvigl/dir_light_node.h>
#include <dvigl/point_light_node.h>
#include <dvigl/spot_light_node.h>

#include <dvigl/physics_manager.h>

InputMgr gInputMgr;

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

    if (keystates[SDL_SCANCODE_A])
    {
        cam->move_left(movement_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_D])
    {
        cam->move_right(movement_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_F])
    {
        cam->position.y -= movement_speed * time_delta;
    }

    if (keystates[SDL_SCANCODE_R])
    {
        cam->position.y += movement_speed * time_delta;
    }

    if (keystates[SDL_SCANCODE_LEFT])
    {
        cam->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), rotation_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_RIGHT])
    {
        cam->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), -rotation_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_DOWN])
    {
        cam->rotate_around_vector(cam->left, rotation_speed * time_delta);
    }
    if (keystates[SDL_SCANCODE_UP])
    {
        cam->rotate_around_vector(cam->left, -rotation_speed * time_delta);
    }

    if (keystates[SDL_SCANCODE_SPACE])
    {

        btCollisionObject* obj = PhysicsMgr::ptr()->dynamicsWorld->getCollisionObjectArray()[1];
        btRigidBody* body = btRigidBody::upcast(obj);
        body->activate(true);
        body->applyCentralImpulse(btVector3(0.f, 5.0f, 0.f));
    }

    if (keystates[SDL_SCANCODE_9])
    {
        RenderMgr::ptr()->fov += 0.001 * time_delta;
    }
    if (keystates[SDL_SCANCODE_0])
    {
        RenderMgr::ptr()->fov -= 0.001 * time_delta;
    }
}

void InputMgr::mouse_motion(SDL_MouseMotionEvent* event)
{
    // LOG("mouse motion %d, %d\n", event->x, event->y);
    // LOG("mouse motion %d, %d\n", event->xrel, event->yrel);

    // CameraNode* cam = SceneMgr::ptr()->get_current_scene()->get_current_camera();

    // float speed = 0.005;

    // cam->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), -event->xrel * speed);
    // cam->rotate_around_vector(cam->left, event->yrel * speed);
}

bool InputMgr::init()
{
    // if (SDL_SetRelativeMouseMode(SDL_TRUE))
    // {
    //     LOG("SDL_SetRelativeMouseMode failed: %s\n", SDL_GetError());
    //     return false;
    // }
    return true;
}

void InputMgr::release() {}