#include <dvigl/file_system_manager.h>
#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>
#include <dvigl/skinned_model_node.h>

ModelMgr gModelMgr;

bool ModelMgr::init() { return true; }

bool ModelMgr::load_skinned_model(std::string name, std::string file_name, std::string format, float scale)
{
    file_name = "../res/models/" + file_name;
    LOG("Loading skinned model %s\n", file_name.c_str());
    auto it = skinned_models.find(name);
    while (it != skinned_models.end())
    {
        it->second->release();
        skinned_models.erase(it);
        it = skinned_models.find(name);
    }
    char* content = FileSystemMgr::ptr()->get_content(file_name);
    int size = FileSystemMgr::ptr()->get_size(file_name);
    if (!content)
    {
        return false;
    }
    skinned_models[name] = new SkinnedModelNode(content, size, format, scale);
    free(content);
    return true;
}

bool ModelMgr::load_model(std::string name, std::string file_name, std::string format, float scale)
{
    file_name = "../res/models/" + file_name;
    LOG("Loading static model %s\n", file_name.c_str());
    auto it = models.find(name);
    while (it != models.end())
    {
        it->second->release();
        models.erase(it);
        it = models.find(name);
    }
    char* content = FileSystemMgr::ptr()->get_content(file_name);
    int size = FileSystemMgr::ptr()->get_size(file_name);
    if (!content)
    {
        return false;
    }
    models[name] = new ModelNode(content, size, format, scale);
    free(content);
    return true;
}

// bool ModelMgr::generate_model(std::string name, SDL_Surface *surf) {
//   auto it = models.find(name);
//   while (it != models.end()) {
//     it->second->release();
//     models.erase(it);
//     it = models.find(name);
//   }
//   models[name] = new ModelNode(surf->w * 0.1f, surf->h * 0.1f);
//   return true;
// }

bool ModelMgr::generate_plane_model(std::string name, int w, int h, std::string material)
{
    auto it = models.find(name);
    while (it != models.end())
    {
        it->second->release();
        models.erase(it);
        it = models.find(name);
    }
    models[name] = new ModelNode(w, h, material);
    return true;
}

ModelNode* ModelMgr::get_model(std::string name)
{
    auto it = models.find(name);
    if (it == models.end())
    {
        LOG("Model %s not found\n", name.c_str());
        return NULL;
    }
    return models[name];
}

SkinnedModelNode* ModelMgr::get_skinned_model(std::string name)
{
    auto it = skinned_models.find(name);
    if (it == skinned_models.end())
    {
        LOG("SkinnedModel %s not found\n", name.c_str());
        return NULL;
    }
    return skinned_models[name];
}

void ModelMgr::release()
{
    // release ttf fonts
    for (auto& pair : models)
    {
        // LOG("Free model %s\n", pair.first.c_str() );
        pair.second->release();
        delete pair.second;
    }
    // release all models
}
