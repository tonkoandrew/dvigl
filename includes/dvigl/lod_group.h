#pragma once
#include <dvigl/core.h>
#include <dvigl/node.h>

class ModelNode;


class LODGroup : public Node
{
public:
    LODGroup(int lod_count);
    void add_model_lod(ModelNode* model, int level, float max_distance);
    void update(float distance);
    void release();
    std::unordered_map<int, ModelNode*> models;
    std::unordered_map<int, float> levels;
};
