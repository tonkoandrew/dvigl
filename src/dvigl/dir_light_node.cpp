#include <dvigl/dir_light_node.h>

DirLightNode::DirLightNode(glm::vec3 rot, glm::vec3 clr, float intnst)
{
    forward = glm::vec3(0.0f, -1.0f, 0.0f);
    up = glm::vec3(0.0f, 0.0f, -1.0f);
    left = glm::cross(up, forward);
    set_rotation(rot);

    color = clr;
    intensity = intnst;
}

glm::vec3 DirLightNode::get_direction()
{
    return forward;
}