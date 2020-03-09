#include <dvigl/scene.h>
#include <dvigl/camera_node.h>

Scene::Scene()
{
    current_camera = new CameraNode();
    // current_camera->pitch(45.0f);
    current_camera->move_back(100.0f);
    current_camera->move_up(20.0f);

    // current_camera->yaw(-0.54f);
}

CameraNode * Scene::get_current_camera(void) { return current_camera; }
