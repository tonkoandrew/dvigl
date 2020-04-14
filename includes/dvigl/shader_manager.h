#pragma once
#include <dvigl/core.h>

class Shader;

class ShaderMgr : public Singleton<ShaderMgr>
{
public:
    bool init();
    bool load_shader(std::string name, std::string file_name);
    std::unordered_map<std::string, std::string> preProcessShaderBinary(std::string& source);
    Shader* get_shader(std::string name);
    void release();

private:
    std::unordered_map<std::string, Shader*> shaders;
};
