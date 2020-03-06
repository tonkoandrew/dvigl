#pragma once
#include <dvigl/core.h>

class ModelNode;

class ModelMgr : public Singleton<ModelMgr>
{
  public:
    bool init();
    ModelNode * get_model(std::string name);
    bool load_model(std::string name, std::string file_name);
    bool generate_model(std::string name, SDL_Surface * surf);
    void release();

  private:
    std::map<std::string, ModelNode *> models;
};