#include <dvigl/scene_manager.h>
#include <dvigl/scene.h>
#include <dvigl/shader_manager.h>
#include <dvigl/audio_manager.h>
#include <dvigl/audio.h>
#include <dvigl/texture_manager.h>
#include <dvigl/texture.h>

#include <dvigl/font_manager.h>
#include <dvigl/model_manager.h>
#include <dvigl/model_node.h>

#include <dvigl/camera_node.h>

SceneMgr gSceneMgr;

bool SceneMgr::init()
{
    current_scene = new Scene();
    return true;
}

bool SceneMgr::load_scene(std::string file_name)
{

    if (!ShaderMgr::ptr()->load_shader("simple", "../res/shaders/simple.vs", "../res/shaders/simple.fs"))
    {
        return false;
    }

    // AudioMgr::ptr()->set_volume(MIX_MAX_VOLUME / 2);

    // if (!AudioMgr::ptr()->load_audio("voodoo_people", "../res/audio/voodoo_chile.ogg"))
    // {
    //     return false;
    // }

    // AudioMgr::ptr()->get_audio("voodoo_people")->play();



    // if (!TextureMgr::ptr()->load_texture("yoda", "res/textures/yoda.png"))
    // if (!TextureMgr::ptr()->load_texture("yoda", "../res/textures/guard1_body.tga"))
    // if (!TextureMgr::ptr()->load_texture("yoda", "../res/textures/guard1_face.tga"))
    // {
    //     return false;
    // }


    if (!FontMgr::ptr()->load_font("FreeSans_150", "../res/fonts/Magician.ttf", 150))
    {
        return false;
    }

    // if (!ModelMgr::ptr()->load_model("yoda", "../res/models/boblampclean.md5anim", "md5anim"))
    if (!ModelMgr::ptr()->load_model("yoda", "../res/models/boblampclean.md5mesh", "md5mesh"))
    // if (!ModelMgr::ptr()->load_model("yoda", "../res/models/yoda.3ds", "3ds"))
    // if (!ModelMgr::ptr()->load_model("yoda", "../res/models/yoda.dae", "collada"))
    {
        return false;
    }

    // if (!ModelMgr::ptr()->generate_plane_model("plane", 100, 100))
    // {
    //     return false;
    // }

    ModelNode * m = ModelMgr::ptr()->get_model("yoda");
    if (!m)
    {
        return false;
    }

    // m->move_forward(180.0f);
    m->move_down(20.0f);
    m->move_right(120.0f);
    m->pitch(-3.14f / 2.0f);
    m->roll(-3.14f);


    // m = ModelMgr::ptr()->get_model("plane");
    // // m->move_down(20.0f);
    // // m->move_right(120.0f);
    // m->move_back(20.0f);
    // m->move_up(20.0f);
    // m->roll(-3.14f);
    // // m->yaw(3.14f/2.0f);
    
    return true;
}

void SceneMgr::update(float time_delta)
{
    // current_scene->get_current_camera()->move_forward(time_delta * 0.01);

    ModelNode * m = ModelMgr::ptr()->get_model("yoda");
    m->roll(0.0003 * time_delta);

    // m = ModelMgr::ptr()->get_model("plane");
    // m->move_forward(0.01* time_delta);
}

Scene * SceneMgr::get_current_scene()
{
    return current_scene;
}

void SceneMgr::release()
{
    // Release all scenes
}
