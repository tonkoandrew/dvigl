#include <dvigl/audio.h>
#include <dvigl/audio_manager.h>
#include <dvigl/scene.h>
#include <dvigl/scene_manager.h>
#include <dvigl/shader_manager.h>
#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

#include <dvigl/font_manager.h>
#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>
#include <dvigl/skinned_model_node.h>

#include <dvigl/camera_node.h>
#include <dvigl/file_system_manager.h>
#include <dvigl/video_manager.h>

SceneMgr gSceneMgr;

bool SceneMgr::init()
{
    current_scene = new Scene();

    return true;
}

bool SceneMgr::load_scene(std::string file_name)
{

    LOG("Loading Scene %s\n", file_name.c_str());

    char* content = FileSystemMgr::ptr()->get_content(file_name);
    YAML::Node node = YAML::Load(content);
 
    if (!ShaderMgr::ptr()->load_shader("simple", "../res/shaders/simple.vs",
            "../res/shaders/simple.fs")) {
        return false;
    }

    if (!ShaderMgr::ptr()->load_shader("skinned", "../res/shaders/skinned.vs",
            "../res/shaders/skinned.fs")) {
        return false;
    }

    if (!FontMgr::ptr()->load_font("FreeSans_150", "../res/fonts/Magician.ttf", 150)) {
        return false;
    }

    // VideoMgr::ptr()->load_video("../res/videos/video.ogv");
    TextureMgr::ptr()->load_texture("../res/textures/dirt_seamless.jpg", "../res/textures/dirt_seamless.jpg");

    if (!ModelMgr::ptr()->generate_plane_model("plane", 200, 200, "../res/textures/dirt_seamless.jpg")) {
        return false;
    }
    ModelNode* plane = ModelMgr::ptr()->get_model("plane");
    plane->set_rotation(glm::vec3(3.14 / 2, 0.0, 0.0));
    plane->set_position(glm::vec3(-100.0f, 0.0, -50.0));


    if (node["static_models"]) {
        if (node["static_models"].Type() != YAML::NodeType::Sequence) {
            LOG("static_models should be list of objects\n");
            return false;
        }

        for (std::size_t i = 0; i < node["static_models"].size(); i++) {
            YAML::Node model = node["static_models"][i];
            assert(model.Type() == YAML::NodeType::Map);
            assert(model["file"].Type() == YAML::NodeType::Scalar);
            std::string name = model["name"].as<std::string>();
            std::string fname = model["file"].as<std::string>();
            std::string format = model["format"].as<std::string>();
            float scale = model["scale"].as<float>();

            if (!ModelMgr::ptr()->load_model(name, fname, format, scale)) {
                return false;
            }

            YAML::Node pos_node = model["position"];
            glm::vec3 position = glm::vec3(pos_node["x"].as<float>(), pos_node["y"].as<float>(), pos_node["z"].as<float>());

            YAML::Node rot_node = model["rotation"];
            glm::vec3 rotation = glm::vec3(rot_node["x"].as<float>(), rot_node["y"].as<float>(), rot_node["z"].as<float>());

            ModelNode* m = ModelMgr::ptr()->get_model(name);
            m->set_position(position);
            m->set_rotation(rotation);
        }
    }

    if (node["skinned_models"]) {
        if (node["skinned_models"].Type() != YAML::NodeType::Sequence) {
            LOG("skinned_models should be list of objects\n");
            return false;
        }

        for (std::size_t i = 0; i < node["skinned_models"].size(); i++) {
            YAML::Node model = node["skinned_models"][i];
            assert(model.Type() == YAML::NodeType::Map);
            assert(model["file"].Type() == YAML::NodeType::Scalar);
            std::string name = model["name"].as<std::string>();
            std::string fname = model["file"].as<std::string>();
            std::string format = model["format"].as<std::string>();
            float scale = model["scale"].as<float>();

            if (!ModelMgr::ptr()->load_skinned_model(name, fname, format, scale)) {
                return false;
            }
            YAML::Node pos_node = model["position"];
            glm::vec3 position = glm::vec3(pos_node["x"].as<float>(), pos_node["y"].as<float>(), pos_node["z"].as<float>());

            YAML::Node rot_node = model["rotation"];
            glm::vec3 rotation = glm::vec3(rot_node["x"].as<float>(), rot_node["y"].as<float>(), rot_node["z"].as<float>());

            SkinnedModelNode* m = ModelMgr::ptr()->get_skinned_model(name);
            m->set_position(position);
            m->set_rotation(rotation);
        }
    }

    current_scene->get_current_camera()->move_back(200.0f);
    current_scene->get_current_camera()->move_up(80.0f);

    AudioMgr::ptr()->set_volume(MIX_MAX_VOLUME / 2);
    // if (!AudioMgr::ptr()->load_audio("elvis", "../res/audio/elvis.ogg"))
    // {
    //     return false;
    // }

    // AudioMgr::ptr()->get_audio("elvis")->play();

    return true;
}

void SceneMgr::update(float time_delta)
{
    SkinnedModelNode* elvis = ModelMgr::ptr()->get_skinned_model("elvis");
}

Scene* SceneMgr::get_current_scene()
{
    return current_scene;
}

void SceneMgr::release()
{
    // Release all scenes
}
