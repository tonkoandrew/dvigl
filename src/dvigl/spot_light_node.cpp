#include <dvigl/spot_light_node.h>

SpotLightNode::SpotLightNode(
    glm::vec3 pos, glm::vec3 rot, glm::vec3 clr, float intnst, float attRad, float ctf, float octf)
{
    position = pos;

    forward = glm::vec3(0.0f, -1.0f, 0.0f);
    up = glm::vec3(0.0f, 0.0f, -1.0f);
    left = glm::cross(up, forward);
    set_rotation(rot);

    color = clr;
    intensity = intnst;
    attenuationRadius = attRad;

    cutOff = ctf;
    outerCutOff = octf;
    ;
}

glm::vec3 SpotLightNode::get_direction() { return forward; }