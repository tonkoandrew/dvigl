#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class DirLightNode : public Node
{
public:
    DirLightNode(glm::vec3 rot, glm::vec3 color, float intensity);
    glm::vec3 get_direction();
    glm::vec3 color;
    float intensity;
};
