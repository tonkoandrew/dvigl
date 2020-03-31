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
#include <dvigl/video_manager.h>

SceneMgr gSceneMgr;

bool SceneMgr::init() {
  current_scene = new Scene();

  return true;
}

bool SceneMgr::load_scene(std::string file_name) {

  LOG("Loading Scene %s\n", file_name.c_str());

  if (!ShaderMgr::ptr()->load_shader("simple", "../res/shaders/simple.vs",
                                     "../res/shaders/simple.fs")) {
    return false;
  }

LOG("ShaderMgr loaded shader simple\n");
  if (!ShaderMgr::ptr()->load_shader("skinned", "../res/shaders/skinned.vs",
                                     "../res/shaders/skinned.fs")) {
    return false;
  }


  // if (!TextureMgr::ptr()->load_texture("yoda", "res/textures/yoda.png"))
  // if (!TextureMgr::ptr()->load_texture("yoda",
  // "../res/textures/guard1_body.tga"))
  // if (!TextureMgr::ptr()->load_texture("yoda",
  // "../res/textures/guard1_face.tga"))
  // {
  //     return false;
  // }

  if (!FontMgr::ptr()->load_font("FreeSans_150", "../res/fonts/Magician.ttf",
                                 150)) {
    return false;
  }


  // if (!ModelMgr::ptr()->load_model("yoda", "../res/models/yoda.dae", "collada", 20.0f))

  // if (!ModelMgr::ptr()->load_model("yoda", "../res/models/boblampclean.md5mesh", "md5mesh", 1.5f))
  if (!ModelMgr::ptr()->load_model("yoda", "../res/models/bob.dae", "collada", 1.5f))
  {
    return false;
  }

  // if (!ModelMgr::ptr()->load_skinned_model("elvis", "../res/models/bob.dae", "collada", 1.5f))
  if (!ModelMgr::ptr()->load_skinned_model("elvis", "../res/models/elvis.dae", "collada", 50.5f))
  // if (!ModelMgr::ptr()->load_skinned_model("elvis", "../res/models/yoda.dae", "collada", 20.0f))
  // if (!ModelMgr::ptr()->load_skinned_model("elvis", "../res/models/samba_dancing.dae", "collada", 0.5f))
  {
    return false;
  }

  if (!ModelMgr::ptr()->load_skinned_model("bob", "../res/models/bob.dae", "collada", 1.5f))
  {
    return false;
  }

  VideoMgr::ptr()->load_video("../res/videos/video.ogv");


  // if (!ModelMgr::ptr()->generate_plane_model("plane", 100, 100))
  // {
  //     return false;
  // }

  ModelNode *yoda = ModelMgr::ptr()->get_model("yoda");
  if (!yoda) {
    return false;
  }

  // yoda->move_forward(0.0f);
  yoda->move_right(100.0f);
  yoda->pitch(-3.14f/2);
  yoda->roll(-3.14f + 1.0f);

  SkinnedModelNode *elvis = ModelMgr::ptr()->get_skinned_model("elvis");
  if (!elvis) {
    LOG("NO ELVIS FOUND");
    return false;
  }
  elvis->yaw(-3.14f);
  elvis->pitch(-3.14f/2);

  SkinnedModelNode *bob = ModelMgr::ptr()->get_skinned_model("bob");
  if (!bob) {
    LOG("NO bob FOUND");
    return false;
  }
  bob->move_right(100.0f);
  bob->yaw(-3.14f);
  bob->pitch(-3.14f/2);

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

void SceneMgr::update(float time_delta) {
  SkinnedModelNode *elvis = ModelMgr::ptr()->get_skinned_model("elvis");
  // elvis->pitch(0.003 * time_delta);
  // elvis->yaw(0.003 * time_delta);
  // elvis->move_back(0.1 * time_delta);

  // current_scene->get_current_camera()->move_back(time_delta * 0.01);

  // ModelNode * m = ModelMgr::ptr()->get_model("yoda");
  // m->pitch(0.0003 * time_delta);

  // m = ModelMgr::ptr()->get_model("plane");
  // m->move_forward(0.01* time_delta);
}

Scene *SceneMgr::get_current_scene() { return current_scene; }

void SceneMgr::release() {
  // Release all scenes
}
