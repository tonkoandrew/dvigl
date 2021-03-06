#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class CameraNode : public Node
{
public:
    CameraNode();
    glm::mat4 get_view_matrix();

    glm::mat4 prev_view_matrix;
};
