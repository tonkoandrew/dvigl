#include <dvigl/file_system_manager.h>
#include <dvigl/material_manager.h>

MaterialMgr gMaterialMgr;

bool MaterialMgr::init() { return true; }
void MaterialMgr::release()
{
    // release materials
}

bool MaterialMgr::load_material(std::string file_name)
{
    LOG("LOADING MATERIAL: %s\n", file_name.c_str());

    char* content = FileSystemMgr::ptr()->get_content(file_name);

    YAML::Node node = YAML::Load(content);

    // LOG("NODE TYPE: ");
    // switch (node.Type()) {
    //   case YAML::NodeType::Null: LOG("Null\n"); break;
    //   case YAML::NodeType::Scalar: LOG("Scalar\n"); break;
    //   case YAML::NodeType::Sequence: LOG("Sequence\n"); break;
    //   case YAML::NodeType::Map: LOG("Map\n"); break;
    //   case YAML::NodeType::Undefined: LOG("Undefined\n"); break;
    //   default:
    //    LOG("WTF???\n");
    // }

    if (node["diffuse"]) {
        LOG("DIFFUSE MAP: %s\n", node["diffuse"].as<std::string>().c_str());
    }

    if (node["normal"]) {
        LOG("NORMAL MAP: %s\n", node["normal"].as<std::string>().c_str());
    }

    if (node["uv_params"]) {
        if (node["uv_params"]["offset_x"]) {

            LOG("offset_x: %f\n", node["uv_params"]["offset_x"].as<float>());
        }
        if (node["uv_params"]["offset_y"]) {
            LOG("offset_y: %f\n", node["uv_params"]["offset_y"].as<float>());
        }
    }

    // for (std::size_t i=0; i< node.size(); i++) {
    //   LOG("%s\n", node[i].as<int>());
    // }
    return true;
}