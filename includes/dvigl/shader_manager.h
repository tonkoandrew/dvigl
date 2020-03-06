#pragma once
#include <dvigl/core.h>

class Shader;

class ShaderMgr : public Singleton<ShaderMgr>
{
  public:
    bool init();
    bool load_shader(std::string name, std::string vs_file_name, std::string fs_file_name);
    Shader * get_shader(std::string name);
    void release();

  private:
    std::map<std::string, Shader *> shaders;
};
