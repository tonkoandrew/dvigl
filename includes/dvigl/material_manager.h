#pragma once
#include <dvigl/core.h>

class Material;

class MaterialMgr : public Singleton<MaterialMgr>
{
public:
    bool init();
    bool load_material(std::string name, std::string path);
    void release();
    Material* get_material(std::string name);

private:
    std::map<std::string, Material*> materials;
};