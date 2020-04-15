#include <dvigl/application.h>
#include <dvigl/input_manager.h>

#include <dvigl/camera_node.h>
#include <dvigl/scene.h>
#include <dvigl/scene_manager.h>

#include <dvigl/model_manager.h>
#include <dvigl/skinned_model_node.h>

#include <dvigl/render_manager.h>

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

    float movement_speed = 0.05f;
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

    if (keystates[SDL_SCANCODE_UP])
    {
        cam->rotate_around_vector(cam->left, rotation_speed * time_delta);
    }
    if (keystates[SDL_SCANCODE_DOWN])
    {
        cam->rotate_around_vector(cam->left, -rotation_speed * time_delta);
    }

    // Node* model = (Node*)ModelMgr::ptr()->get_skinned_model("elvis");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("bob");
    Node* model = (Node*)ModelMgr::ptr()->get_model("sphere");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("plane");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("body");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("base");
    // Node* model = (Node*)ModelMgr::ptr()->get_model("head");

    if (keystates[SDL_SCANCODE_T])
    {
        model->position.z += movement_speed * time_delta;
        // model->move_up(movement_speed * time_delta);
    }
    if (keystates[SDL_SCANCODE_G])
    {
        model->position.z -= movement_speed * time_delta;
        // model->move_down(movement_speed * time_delta);
    }
    // if (keystates[SDL_SCANCODE_LEFT])
    // {
    //     model->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), -rotation_speed * time_delta);
    //     // model->move_left(movement_speed * time_delta);
    // }
    // if (keystates[SDL_SCANCODE_RIGHT])
    // {
    //     model->rotate_around_vector(glm::vec3(0.0, 1.0, 0.0), rotation_speed * time_delta);
    // }
    // if (keystates[SDL_SCANCODE_UP])
    // {
    //     model->rotate_around_vector(glm::vec3(1.0, 0.0, 0.0), rotation_speed * time_delta);
    // }
    // if (keystates[SDL_SCANCODE_DOWN])
    // {
    //     model->rotate_around_vector(glm::vec3(1.0, 0.0, 0.0), -rotation_speed * time_delta);
    // }
    if (keystates[SDL_SCANCODE_Y])
    {
        model->position.y += movement_speed * time_delta;
    }
    if (keystates[SDL_SCANCODE_H])
    {
        model->position.y -= movement_speed * time_delta;
    }
    if (keystates[SDL_SCANCODE_U])
    {
        model->position.x += movement_speed * time_delta;
    }
    if (keystates[SDL_SCANCODE_J])
    {
        model->position.x -= movement_speed * time_delta;
    }

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

    if (keystates[SDL_SCANCODE_P])
    {
        RenderMgr::ptr()->visualize_world_position = 1;
    }
    else
    {
        RenderMgr::ptr()->visualize_world_position = 0;
    }


    if (keystates[SDL_SCANCODE_V])
    {
        RenderMgr::ptr()->cutOff += 0.0001 * time_delta;
        if (RenderMgr::ptr()->cutOff > 1) 
            RenderMgr::ptr()->cutOff = 1;
        if(RenderMgr::ptr()->outerCutOff > RenderMgr::ptr()->cutOff)
            RenderMgr::ptr()->outerCutOff  -= 0.0001 * time_delta;
        LOG("cutOff = %f   outerCutOff =  %f\n", RenderMgr::ptr()->cutOff, RenderMgr::ptr()->outerCutOff);
    }
    if (keystates[SDL_SCANCODE_X])
    {
        RenderMgr::ptr()->cutOff -= 0.0001 * time_delta;
        if (RenderMgr::ptr()->cutOff < 0)
            RenderMgr::ptr()->cutOff = 0;
        if(RenderMgr::ptr()->outerCutOff > RenderMgr::ptr()->cutOff)
            RenderMgr::ptr()->outerCutOff  -= 0.0001 * time_delta;
        LOG("cutOff = %f   outerCutOff =  %f\n", RenderMgr::ptr()->cutOff, RenderMgr::ptr()->outerCutOff);
    }

    if (keystates[SDL_SCANCODE_L])
    {
        RenderMgr::ptr()->outerCutOff += 0.0001 * time_delta;
        if(RenderMgr::ptr()->outerCutOff > RenderMgr::ptr()->cutOff)
            RenderMgr::ptr()->outerCutOff -= 0.0001 * time_delta;
        LOG("cutOff = %f   outerCutOff =  %f\n", RenderMgr::ptr()->cutOff, RenderMgr::ptr()->outerCutOff);
    }
    if (keystates[SDL_SCANCODE_K])
    {
        RenderMgr::ptr()->outerCutOff -= 0.0001 * time_delta;

        if(RenderMgr::ptr()->outerCutOff < 0)
            RenderMgr::ptr()->outerCutOff  = 0;
        LOG("cutOff = %f   outerCutOff =  %f\n", RenderMgr::ptr()->cutOff, RenderMgr::ptr()->outerCutOff);
    }

    
}

void InputMgr::release() {}