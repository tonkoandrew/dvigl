#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class Mesh;

class ModelNode : public Node
{
public:
    ModelNode(char* content, int content_size, std::string format, float scale);
    ModelNode(int w, int h, std::string texture);
    void draw();
    void release();
    float get_bounding_sphere_radius();

    bool visible = true;
private:
    std::vector<Mesh*> meshes;
    float bounding_radius = 0.0f;
};
