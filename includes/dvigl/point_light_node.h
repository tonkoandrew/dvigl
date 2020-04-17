#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class PointLightNode : public Node
{
public:
    PointLightNode(glm::vec3 position, glm::vec3 color, float intensity, float attenuationRadius);
    glm::vec3 color;
    float intensity;
    float attenuationRadius;
};
