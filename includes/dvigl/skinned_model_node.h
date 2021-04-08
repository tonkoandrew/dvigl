#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class SkinnedMesh;
class Shader;

class SkinnedModelNode : public Node
{
public:
    SkinnedModelNode(char* content, int content_size, std::string format, float scale);
    void update(float time_delta);
    void draw(Shader* s);
    void release();
    float get_bounding_sphere_radius();

    float m_startTime = 0.0f;

private:
    float bounding_radius = 0.0f;
    std::vector<SkinnedMesh*> meshes;
};
