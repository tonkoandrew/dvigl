#include <dvigl/camera_node.h>
CameraNode::CameraNode()
{
    forward = glm::vec3(0.0f, -1.0f, 0.0f);
    up = glm::vec3(0.0f, 0.0f, -1.0f);
    left = glm::cross(up, forward);
    position = glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::mat4 CameraNode::get_view_matrix()
{
    glm::vec3 target = position + forward;
    return glm::lookAt(position, target, up);
}
