#include <dvigl/lod_group.h>
#include <dvigl/model_node.h>

LODGroup::LODGroup(int lod_count)
{
    levels.reserve(lod_count);
    models.reserve(lod_count);
}

void LODGroup::add_model_lod(ModelNode* model, int level, float max_distance)
{
    levels[level] = max_distance;
    models[level] = model;
    model->visible = false;
}

void LODGroup::update(float distance)
{
    int current_level = 0;

    for (int i = 0; i < models.size(); i++)
    {
        models[i]->visible = false;
        models[i]->forward = forward;
        models[i]->position = position;
        models[i]->up = up;
        models[i]->left = left;

        if (distance > levels[i])
        {
            current_level = i;
        }
    }

    models[current_level]->visible = true;
    // LOG("dist: %f, level: %d\n", distance, current_level);
}

void LODGroup::release() { }