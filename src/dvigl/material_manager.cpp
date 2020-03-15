#include <dvigl/material_manager.h>
#include <dvigl/file_system_manager.h>

MaterialMgr gMaterialMgr;

bool MaterialMgr::init() { return true; }
void MaterialMgr::release() {
  // release materials
}

bool MaterialMgr::load_material(std::string file_name)
{
    char *content = FileSystemMgr::ptr()->get_content(file_name);

    YAML::Node node = YAML::Load(content);

    LOG("LOADING MATERIAL: %s\n", file_name.c_str());

    if (node["diffuse"])
    {
      LOG("DIFFUSE TEXTURE: %s\n", node["diffuse"].as<std::string>().c_str());
    } 
    // for (std::size_t i=0; i< node.size(); i++) {
    //   LOG("%s\n", node[i].as<int>());
    // }
}