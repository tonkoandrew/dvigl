#include <dvigl/file_system_manager.h>
#include <dvigl/shader.h>
#include <dvigl/shader_manager.h>

ShaderMgr gShaderMgr;

bool ShaderMgr::init() { return true; }
bool ShaderMgr::load_shader(std::string name, std::string vs_file_name,
                            std::string fs_file_name) {
  char *vs_content = FileSystemMgr::ptr()->get_content(vs_file_name);
  char *fs_content = FileSystemMgr::ptr()->get_content(fs_file_name);
  if ((!vs_content) || (!fs_content)) {
    return false;
  }
  Shader *shader = new Shader();
  if (!shader->compile_and_link(vs_content, fs_content)) {
    return false;
  }
  free(vs_content);
  free(fs_content);

  auto it = shaders.find(name);
  while (it != shaders.end()) {
    // LOG("Free shader %s\n", name.c_str());
    it->second->release();
    shaders.erase(it);
    it = shaders.find(name);
  }

  shaders[name] = shader;
  return true;
}

Shader *ShaderMgr::get_shader(std::string name) {
  auto it = shaders.find(name);
  if (it != shaders.end()) {
    return it->second;
  }

  LOG("Shader %s not found\n", name.c_str());
  return NULL;
}

void ShaderMgr::release() {
  // release shaders
  for (auto &pair : shaders) {
    // LOG("Free shaders: %s\n", pair.first.c_str() );
    pair.second->release();
  }
}