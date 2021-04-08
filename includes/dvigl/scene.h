#pragma once
#include <dvigl/core.h>

class CameraNode;
class DirLightNode;
class PointLightNode;
class SpotLightNode;

class Scene
{
public:
    Scene();
    CameraNode* get_current_camera(void);
    std::unordered_map<std::string, DirLightNode*> dir_lights;
    std::unordered_map<std::string, PointLightNode*> point_lights;
    std::unordered_map<std::string, SpotLightNode*> spot_lights;

    CameraNode* current_camera;
// private:
};