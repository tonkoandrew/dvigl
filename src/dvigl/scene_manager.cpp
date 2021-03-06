#include <dvigl/audio.h>
#include <dvigl/audio_manager.h>
#include <dvigl/scene.h>
#include <dvigl/scene_manager.h>
#include <dvigl/shader_manager.h>
#include <dvigl/shader.h>
#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

#include <dvigl/font_manager.h>
#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>
#include <dvigl/skinned_model_node.h>

#include <dvigl/camera_node.h>
#include <dvigl/file_system_manager.h>
#include <dvigl/video_manager.h>

#include <dvigl/material_manager.h>

#include <dvigl/dir_light_node.h>
#include <dvigl/point_light_node.h>
#include <dvigl/spot_light_node.h>

#include <dvigl/lod_group.h>
#include <dvigl/lod_group_manager.h>

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
    free(content);
    if (!ShaderMgr::ptr()->load_shader("static_geometry", "../res/shaders/static_geometry.glsl"))
    {
        return false;
    }

    if (!ShaderMgr::ptr()->load_shader("skinned_geometry", "../res/shaders/skinned_geometry.glsl"))
    {
        return false;
    }

    Shader* s = ShaderMgr::ptr()->get_shader("skinned_geometry");

    if(!s->FindAttribLocation("attr_pos"))
    {
        LOG("OOPS... Can not find attrib 'attr_pos'\n");
        return false;
    }
    s->FindAttribLocation("attr_texcoord");
    s->FindAttribLocation("attr_normal");
    s->FindAttribLocation("attr_bitangent");
    s->FindAttribLocation("attr_tangent");
    
    LOG("attr_pos = %d\n", s->attrib("attr_pos"));
    LOG("attr_texcoord = %d\n", s->attrib("attr_texcoord"));
    LOG("attr_normal = %d\n", s->attrib("attr_normal"));
    LOG("attr_bitangent = %d\n", s->attrib("attr_bitangent"));
    LOG("attr_tangent = %d\n", s->attrib("attr_tangent"));

    if (!ShaderMgr::ptr()->load_shader("deferred", "../res/shaders/deferred.glsl"))
    {
        return false;
    }

    if (!ShaderMgr::ptr()->load_shader("forward", "../res/shaders/forward.glsl"))
    {
        return false;
    }

    if (!ShaderMgr::ptr()->load_shader("shadowmap", "../res/shaders/shadowmap.glsl"))
    {
        return false;
    }

    if (!ShaderMgr::ptr()->load_shader("skinned_shadows", "../res/shaders/skinned_shadows.glsl"))
    {
        return false;
    }

    if (!FontMgr::ptr()->load_font("FreeSans_150", "../res/fonts/Magician.ttf", 150))
    {
        return false;
    }

    LOG("Loading light sources... ");

    if (node["dir_lights"])
    {
        if (node["dir_lights"].Type() != YAML::NodeType::Sequence)
        {
            LOG("dir_lights should be list of objects\n");
            return false;
        }

        for (std::size_t i = 0; i < node["dir_lights"].size(); i++)
        {
            YAML::Node light_node = node["dir_lights"][i];
            glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 color = glm::vec3(1.0f);
            float intensity = 1.0f;

            std::string name = light_node["name"].as<std::string>();

            if (light_node["rotation"])
            {
                float x = light_node["rotation"]["x"].as<float>();
                float y = light_node["rotation"]["y"].as<float>();
                float z = light_node["rotation"]["z"].as<float>();
                rotation = glm::vec3(x, y, z);
            }

            if (light_node["color"])
            {
                float r = light_node["color"]["r"].as<float>();
                float g = light_node["color"]["g"].as<float>();
                float b = light_node["color"]["b"].as<float>();
                color = glm::vec3(r, g, b);
            }
            if (light_node["intensity"])
            {
                intensity = light_node["intensity"].as<float>();
            }

            current_scene->dir_lights[name] = new DirLightNode(rotation, color, intensity);
        }
    }

    if (node["point_lights"])
    {
        if (node["point_lights"].Type() != YAML::NodeType::Sequence)
        {
            LOG("point_lights should be list of objects\n");
            return false;
        }

        for (std::size_t i = 0; i < node["point_lights"].size(); i++)
        {
            YAML::Node light_node = node["point_lights"][i];
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 color = glm::vec3(1.0f);
            float intensity = 1.0f;
            float attenuationRadius = 100.0f;

            std::string name = light_node["name"].as<std::string>();

            if (light_node["position"])
            {
                float x = light_node["position"]["x"].as<float>();
                float y = light_node["position"]["y"].as<float>();
                float z = light_node["position"]["z"].as<float>();
                position = glm::vec3(x, y, z);
            }

            if (light_node["color"])
            {
                float r = light_node["color"]["r"].as<float>();
                float g = light_node["color"]["g"].as<float>();
                float b = light_node["color"]["b"].as<float>();
                color = glm::vec3(r, g, b);
            }
            if (light_node["intensity"])
            {
                intensity = light_node["intensity"].as<float>();
            }
            if (light_node["attenuationRadius"])
            {
                attenuationRadius = light_node["attenuationRadius"].as<float>();
            }

            current_scene->point_lights[name] = new PointLightNode(position, color, intensity, attenuationRadius);
        }
    }

    if (node["spot_lights"])
    {
        if (node["spot_lights"].Type() != YAML::NodeType::Sequence)
        {
            LOG("spot_lights should be list of objects\n");
            return false;
        }

        for (std::size_t i = 0; i < node["spot_lights"].size(); i++)
        {
            YAML::Node light_node = node["spot_lights"][i];
            glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 color = glm::vec3(1.0f);
            float intensity = 1.0f;
            float attenuationRadius = 100.0f;
            float cutOff = 0.1f;
            float outerCutOff = 0.0f;

            std::string name = light_node["name"].as<std::string>();

            if (light_node["position"])
            {
                float x = light_node["position"]["x"].as<float>();
                float y = light_node["position"]["y"].as<float>();
                float z = light_node["position"]["z"].as<float>();
                position = glm::vec3(x, y, z);
            }

            if (light_node["rotation"])
            {
                float x = light_node["rotation"]["x"].as<float>();
                float y = light_node["rotation"]["y"].as<float>();
                float z = light_node["rotation"]["z"].as<float>();
                rotation = glm::vec3(x, y, z);
            }

            if (light_node["color"])
            {
                float r = light_node["color"]["r"].as<float>();
                float g = light_node["color"]["g"].as<float>();
                float b = light_node["color"]["b"].as<float>();
                color = glm::vec3(r, g, b);
            }
            if (light_node["intensity"])
            {
                intensity = light_node["intensity"].as<float>();
            }
            if (light_node["attenuationRadius"])
            {
                attenuationRadius = light_node["attenuationRadius"].as<float>();
            }
            if (light_node["cutOff"])
            {
                cutOff = light_node["cutOff"].as<float>();
            }

            if (light_node["outerCutOff"])
            {
                outerCutOff = light_node["outerCutOff"].as<float>();
            }

            current_scene->spot_lights[name]
                = new SpotLightNode(position, rotation, color, intensity, attenuationRadius, cutOff, outerCutOff);
        }
    }
    LOG("done\n");
    LOG("%d dir lights\n", (int)current_scene->dir_lights.size());
    LOG("%d point lights\n", (int)current_scene->point_lights.size());
    LOG("%d spot lights\n", (int)current_scene->spot_lights.size());

    if (node["static_models"])
    {
        if (node["static_models"].Type() != YAML::NodeType::Sequence)
        {
            LOG("static_models should be list of objects\n");
            return false;
        }

        for (std::size_t i = 0; i < node["static_models"].size(); i++)
        {
            YAML::Node model = node["static_models"][i];
            assert(model.Type() == YAML::NodeType::Map);
            assert(model["file"].Type() == YAML::NodeType::Scalar);
            std::string name = model["name"].as<std::string>();
            std::string fname = model["file"].as<std::string>();
            std::string format = model["format"].as<std::string>();
            float scale = model["scale"].as<float>();

            if (!ModelMgr::ptr()->load_model(name, fname, format, scale))
            {
                return false;
            }

            YAML::Node pos_node = model["position"];
            glm::vec3 position
                = glm::vec3(pos_node["x"].as<float>(), pos_node["y"].as<float>(), pos_node["z"].as<float>());

            YAML::Node rot_node = model["rotation"];
            glm::vec3 rotation
                = glm::vec3(rot_node["x"].as<float>(), rot_node["y"].as<float>(), rot_node["z"].as<float>());

            ModelNode* m = ModelMgr::ptr()->get_model(name);
            m->set_position(position);
            m->set_rotation(rotation);
        }
    }

    if (node["skinned_models"])
    {
        if (node["skinned_models"].Type() != YAML::NodeType::Sequence)
        {
            LOG("skinned_models should be list of objects\n");
            return false;
        }

        for (std::size_t i = 0; i < node["skinned_models"].size(); i++)
        {
            YAML::Node model = node["skinned_models"][i];
            assert(model.Type() == YAML::NodeType::Map);
            assert(model["file"].Type() == YAML::NodeType::Scalar);
            std::string name = model["name"].as<std::string>();
            std::string fname = model["file"].as<std::string>();
            std::string format = model["format"].as<std::string>();
            float scale = model["scale"].as<float>();

            if (!ModelMgr::ptr()->load_skinned_model(name, fname, format, scale))
            {
                return false;
            }
            YAML::Node pos_node = model["position"];
            glm::vec3 position
                = glm::vec3(pos_node["x"].as<float>(), pos_node["y"].as<float>(), pos_node["z"].as<float>());

            YAML::Node rot_node = model["rotation"];
            glm::vec3 rotation
                = glm::vec3(rot_node["x"].as<float>(), rot_node["y"].as<float>(), rot_node["z"].as<float>());

            SkinnedModelNode* m = ModelMgr::ptr()->get_skinned_model(name);
            m->set_position(position);
            m->set_rotation(rotation);
        }
    }

    if (node["lod_groups"])
    {
        if (node["lod_groups"].Type() != YAML::NodeType::Sequence)
        {
            LOG("lod_groups should be list of objects\n");
            return false;
        }
        for (std::size_t i = 0; i < node["lod_groups"].size(); i++)
        {
            YAML::Node lod_group = node["lod_groups"][i];
            assert(lod_group.Type() == YAML::NodeType::Map);
            assert(lod_group["name"].Type() == YAML::NodeType::Scalar);

            std::string name = lod_group["name"].as<std::string>();

            YAML::Node pos_node = lod_group["position"];
            glm::vec3 position
                = glm::vec3(pos_node["x"].as<float>(), pos_node["y"].as<float>(), pos_node["z"].as<float>());

            YAML::Node rot_node = lod_group["rotation"];
            glm::vec3 rotation
                = glm::vec3(rot_node["x"].as<float>(), rot_node["y"].as<float>(), rot_node["z"].as<float>());

            float scale = lod_group["scale"].as<float>();

            int count = lod_group["LODs"].size();

            LOG("LOD group name = %s\n", name.c_str());
            LOG("Levels count = %d\n", count);
            LODGroup* grp = LODGroupMgr::ptr()->create_lod_group(name, count);
            grp->set_position(position);
            grp->set_rotation(rotation);

            for (std::size_t j = 0; j < lod_group["LODs"].size(); j++)
            {
                YAML::Node lod = lod_group["LODs"][j];
                int level = lod["level"].as<int>();
                std::string fname = lod["file"].as<std::string>();
                std::string format = lod["format"].as<std::string>();
                std::string lod_name = lod["name"].as<std::string>();
                float max_dist = lod["max_dist"].as<float>();

                if (!ModelMgr::ptr()->load_model(lod_name, fname, format, scale))
                {
                    return false;
                }
                ModelNode* lodX = ModelMgr::ptr()->get_model(lod_name);
                grp->add_model_lod(lodX, level, max_dist);
            }
        }
    }

    current_scene->current_camera = new CameraNode();
    current_scene->get_current_camera()->set_position(glm::vec3(0.0f, 40.0f, 200.0f));
    current_scene->get_current_camera()->set_rotation(glm::vec3(1.55, 0, 0));
    AudioMgr::ptr()->set_volume(MIX_MAX_VOLUME / 2);
    // if (!AudioMgr::ptr()->load_audio("elvis", "../res/audio/elvis.ogg"))
    // {
    //     return false;
    // }

    // AudioMgr::ptr()->get_audio("elvis")->play();

    // VideoMgr::ptr()->load_video("../res/videos/video.ogv");

    return true;
}

void SceneMgr::update(float time_delta)
{
    // LODGroupMgr::ptr()->lod_groups["suzan"]->roll(time_delta * 0.001f);
    current_scene->spot_lights["spotlight3"]->yaw(time_delta * 0.001f);
}

Scene* SceneMgr::get_current_scene() { return current_scene; }

void SceneMgr::release() {}
