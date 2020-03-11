#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>
#include <dvigl/file_system_manager.h>

ModelMgr gModelMgr;

bool ModelMgr::init() { return true; }
bool ModelMgr::load_model(std::string name, std::string file_name, std::string format)
{
    auto it = models.find(name);
    while (it != models.end())
    {
        it->second->release();
        models.erase(it);
        it = models.find(name);
    }
    char * content = FileSystemMgr::ptr()->get_content(file_name);
    int size = FileSystemMgr::ptr()->get_size(file_name);
    if (!content)
    {
        return false;
    }
    models[name] = new ModelNode(content, size, format);
    return true;
}

bool ModelMgr::generate_model(std::string name, SDL_Surface * surf)
{
    auto it = models.find(name);
    while (it != models.end())
    {
        it->second->release();
        models.erase(it);
        it = models.find(name);
    }
    models[name] = new ModelNode(surf->w * 0.1f, surf->h * 0.1f);
    return true;
}

bool ModelMgr::generate_plane_model(std::string name, int w, int h)
{
    auto it = models.find(name);
    while (it != models.end())
    {
        it->second->release();
        models.erase(it);
        it = models.find(name);
    }
    models[name] = new ModelNode(w * 0.1f, h * 0.1f);
    return true;
}

ModelNode * ModelMgr::get_model(std::string name)
{
    auto it = models.find(name);
    if (it == models.end())
    {
        LOG("Model %s not found\n", name.c_str());
        return NULL;
    }
    return models[name];
}

void ModelMgr::release()
{
    // release ttf fonts
    for (auto & pair : models)
    {
        // LOG("Free model %s\n", pair.first.c_str() );
        pair.second->release();
    }
    // release all models
}
