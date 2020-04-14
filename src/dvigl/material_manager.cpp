#include <dvigl/file_system_manager.h>
#include <dvigl/material.h>
#include <dvigl/material_manager.h>
#include <dvigl/texture_manager.h>

MaterialMgr gMaterialMgr;

bool MaterialMgr::init() { return true; }

void MaterialMgr::release()
{
    // release materials
}

bool MaterialMgr::load_material(std::string name, std::string file_name)
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

    std::string albedo_map;
    std::string normal_map;
    std::string metallic_map;
    std::string roughness_map;
    std::string ao_map;

    if (node["albedo"])
    {
        albedo_map = "../res/textures/" + node["albedo"].as<std::string>();
        TextureMgr::ptr()->load_texture(albedo_map, albedo_map);
    }

    if (node["normal"])
    {
        normal_map = "../res/textures/" + node["normal"].as<std::string>();
        TextureMgr::ptr()->load_texture(normal_map, normal_map);
    }

    if (node["metallic"])
    {
        metallic_map = "../res/textures/" + node["metallic"].as<std::string>();
        TextureMgr::ptr()->load_texture(metallic_map, metallic_map);
    }

    if (node["roughness"])
    {
        roughness_map = "../res/textures/" + node["roughness"].as<std::string>();
        TextureMgr::ptr()->load_texture(roughness_map, roughness_map);
    }
    if (node["ao"])
    {
        ao_map = "../res/textures/" + node["ao"].as<std::string>();
        TextureMgr::ptr()->load_texture(ao_map, ao_map);
    }

    // if (node["uv_params"]) {
    //     if (node["uv_params"]["offset_x"]) {

    //         LOG("offset_x: %f\n", node["uv_params"]["offset_x"].as<float>());
    //     }
    //     if (node["uv_params"]["offset_y"]) {
    //         LOG("offset_y: %f\n", node["uv_params"]["offset_y"].as<float>());
    //     }
    // }

    // for (std::size_t i=0; i< node.size(); i++) {
    //   LOG("%s\n", node[i].as<int>());
    // }
    Texture* albedo_tex = TextureMgr::ptr()->get_texture(albedo_map);
    Texture* normal_tex = TextureMgr::ptr()->get_texture(normal_map);
    Texture* metallic_tex = TextureMgr::ptr()->get_texture(metallic_map);
    Texture* roughness_tex = TextureMgr::ptr()->get_texture(metallic_map);
    Texture* ao_tex = TextureMgr::ptr()->get_texture(ao_map);
    materials[name] = new Material(albedo_tex, normal_tex, metallic_tex, roughness_tex, ao_tex);
    return true;
}

Material* MaterialMgr::get_material(std::string name)
{
    auto it = materials.find(name);
    if (it != materials.end())
    {
        return it->second;
    }

    LOG("Material %s not found\n", name.c_str());
    return NULL;
}
