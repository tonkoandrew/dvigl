#include <dvigl/camera_node.h>

glm::mat4 CameraNode::get_view_matrix()
{
    glm::vec3 target = position + forward;
    return glm::lookAt(position, target, up);
}
