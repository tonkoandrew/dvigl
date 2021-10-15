#pragma once
#include <dvigl/core.h>

class ModelNode;
class SkinnedModelNode;

class ModelMgr : public Singleton<ModelMgr>
{
public:
    bool init();
    ModelNode* get_model(std::string name);
    SkinnedModelNode* get_skinned_model(std::string name);
    bool load_model(std::string name, std::string file_name, std::string format, float scale);
    bool load_skinned_model(std::string name, std::string file_name, std::string format, float scale);
    bool generate_model(std::string name, SDL_Surface* surf);
    void release();

    void update_animation_timers(float time_delta);

    std::unordered_map<std::string, ModelNode*> models;
    std::unordered_map<std::string, SkinnedModelNode*> skinned_models;

    float animation_speed = 1.0f;

private:
};