#include <dvigl/camera_node.h>
#include <dvigl/scene.h>

Scene::Scene() { current_camera = new CameraNode(); }

CameraNode* Scene::get_current_camera(void) { return current_camera; }
