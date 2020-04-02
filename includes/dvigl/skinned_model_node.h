#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class SkinnedMesh;

class SkinnedModelNode : public Node {
public:
    SkinnedModelNode(char* content, int content_size, std::string format, float scale);
    void draw(glm::mat4 mvp);
    void release();

private:
    std::vector<SkinnedMesh*> meshes;
};
