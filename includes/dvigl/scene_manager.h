#pragma once
#include <dvigl/core.h>
#include <dvigl/skinned_mesh.h>

class Scene;

class SceneMgr : public Singleton<SceneMgr> {
public:
  bool init();
  bool load_scene(std::string file_name);
  void update(float time_delta);
  Scene *get_current_scene();
  void release();

  SkinnedMesh m_mesh;

private:
  Scene *current_scene;
  std::vector<Scene *> loaded_scenes;
};
