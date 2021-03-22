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
        // cam->position -= glm::normalize(glm::vec3(cam->forward.x, 0, cam->forward.z)) * movement_speed * time_delta;
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
        body->applyCentralImpulse(btVector3(0.f, 0.8f, 0.f));
    }
    // Node* model = (Node*)ModelMgr::ptr()->get_skinned_model("elvis");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("bob");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("sphere");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("plane");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("body");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("base");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("flame_head");

    // SpotLightNode* model = (SpotLightNode*)SceneMgr::ptr()->get_current_scene()->spot_lights["spotlight1"];
    // model->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), rotation_speed * time_delta);
    // // model = (SpotLightNode*)SceneMgr::ptr()->get_current_scene()->spot_lights["spotlight2"];
    // model->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), rotation_speed * time_delta);
    // // model = (SpotLightNode*)SceneMgr::ptr()->get_current_scene()->spot_lights["spotlight3"];
    // model->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), rotation_speed * time_delta);

    // model = (SpotLightNode*)SceneMgr::ptr()->get_current_scene()->spot_lights["flame_head"];

    // if (keystates[SDL_SCANCODE_T])
    // {
    //     model->position.z += movement_speed * time_delta;
    // }
    // if (keystates[SDL_SCANCODE_G])
    // {
    //     model->position.z -= movement_speed * time_delta;
    // }
    // if (keystates[SDL_SCANCODE_Y])
    // {
    //     model->position.y += movement_speed * time_delta;
    // }
    // if (keystates[SDL_SCANCODE_H])
    // {
    //     model->position.y -= movement_speed * time_delta;
    // }
    // if (keystates[SDL_SCANCODE_U])
    // {
    //     model->position.x += movement_speed * time_delta;
    // }
    // if (keystates[SDL_SCANCODE_J])
    // {
    //     model->position.x -= movement_speed * time_delta;
    // }

    if (keystates[SDL_SCANCODE_B])
    {
        RenderMgr::ptr()->visualize_albedo = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_albedo = 0;
    }

    if (keystates[SDL_SCANCODE_N])
    {
        RenderMgr::ptr()->visualize_normals = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_normals = 0;
    }

    if (keystates[SDL_SCANCODE_M])
    {
        RenderMgr::ptr()->visualize_metallic = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_metallic = 0;
    }

    if (keystates[SDL_SCANCODE_I])
    {
        RenderMgr::ptr()->visualize_roughness = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_roughness = 0;
    }

    if (keystates[SDL_SCANCODE_O])
    {
        RenderMgr::ptr()->visualize_ao = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_ao = 0;
    }

    if (keystates[SDL_SCANCODE_V])
    {
        RenderMgr::ptr()->visualize_velocity = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_velocity = 0;
    }

    if (keystates[SDL_SCANCODE_P])
    {
        RenderMgr::ptr()->visualize_world_position = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_world_position = 0;
    }

    // if (keystates[SDL_SCANCODE_X])
    // {
    //     model->pitch(-rotation_speed * time_delta);
    // }
    // if (keystates[SDL_SCANCODE_L])
    // {
    //     model->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), rotation_speed * time_delta);
    // }

    // if (keystates[SDL_SCANCODE_K])
    // {
    //     model->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), -rotation_speed * time_delta);
    // }

    // if (keystates[SDL_SCANCODE_Z])
    // {
    //     model->intensity -= 1.0 * time_delta;
    // }
    if (keystates[SDL_SCANCODE_C])
    {
        RenderMgr::ptr()->visualize_wireframe = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_wireframe = 0;
    }

    // if (keystates[SDL_SCANCODE_1])
    // {
    //     model->attenuationRadius -= 0.1 * time_delta;
    // }
    // if (keystates[SDL_SCANCODE_2])
    // {
    //     model->attenuationRadius += 0.1 * time_delta;
    // }
    if (keystates[SDL_SCANCODE_9])
    {
        RenderMgr::ptr()->fov += 0.001 * time_delta;
    }
    if (keystates[SDL_SCANCODE_0])
    {
        RenderMgr::ptr()->fov -= 0.001 * time_delta;
    }
}

void InputMgr::release() {}