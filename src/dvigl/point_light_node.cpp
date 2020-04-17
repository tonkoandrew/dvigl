#include <dvigl/point_light_node.h>

PointLightNode::PointLightNode(glm::vec3 position, glm::vec3 clr, float intnst, float attRad)
{
    set_position(position);
    color = clr;
    intensity = intnst;
    attenuationRadius = attRad;
}
