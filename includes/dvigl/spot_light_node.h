#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class SpotLightNode : public Node
{
public:
    SpotLightNode(glm::vec3 position, glm::vec3 rot, glm::vec3 color, float intensity, float attenuationRadius,
        float cutOff, float outerCutOff);
    glm::vec3 get_direction();
    glm::vec3 color;
    float intensity;
    float attenuationRadius;
    float cutOff;
    float outerCutOff;
};
