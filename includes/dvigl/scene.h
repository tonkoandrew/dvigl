#pragma once
#include <dvigl/core.h>

class CameraNode;

class Scene {
public:
    Scene();
    CameraNode* get_current_camera(void);

private:
    CameraNode* current_camera;
};