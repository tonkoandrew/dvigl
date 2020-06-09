#include <dvigl/file_system_manager.h>
#include <dvigl/shader.h>
#include <dvigl/shader_manager.h>

ShaderMgr gShaderMgr;

bool ShaderMgr::init() { return true; }

bool ShaderMgr::load_shader(std::string name, std::string file_name)
{
    LOG("Loading shader \"%s\"... ", file_name.c_str());
    char* glsl_content = FileSystemMgr::ptr()->get_content(file_name);
    if ((!glsl_content))
    {
        return false;
    }

    std::string shader_content = std::string(glsl_content);
    auto shaderSources = preProcessShaderBinary(shader_content);
    free(glsl_content);

    Shader* shader = new Shader();
    if (!shader->compile_and_link(shaderSources))
    {
        return false;
    }

    auto it = shaders.find(name);
    while (it != shaders.end())
    {
        // LOG("Free shader %s\n", name.c_str());
        it->second->release();
        shaders.erase(it);
        it = shaders.find(name);
    }

    shaders[name] = shader;
    LOG("done\n");
    return true;
}

std::unordered_map<std::string, std::string> ShaderMgr::preProcessShaderBinary(std::string& source)
{
    std::unordered_map<std::string, std::string> shaderSources;

    const char* shaderTypeToken = "#defshader";
    size_t shaderTypeTokenLength = strlen(shaderTypeToken);
    size_t pos = source.find(shaderTypeToken);
    while (pos != std::string::npos)
    {
        size_t eol = source.find_first_of("\r\n", pos);
        // TODO: eol == std::string::npos, if so then we have a syntax error
        size_t begin = pos + shaderTypeTokenLength + 1;
        std::string shaderType = source.substr(begin, eol - begin);
        // TODO: type != "vertex" || fragment || hull || domain || compute, if so then we have an invalid shader type
        // specified

        size_t nextLinePos = source.find_first_not_of("\r\n", eol);
        pos = source.find(shaderTypeToken, nextLinePos);
        shaderSources[shaderType]
            = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
    }

    return shaderSources;
}

Shader* ShaderMgr::get_shader(std::string name)
{
    auto it = shaders.find(name);
    if (it != shaders.end())
    {
        return it->second;
    }

    LOG("Shader %s not found\n", name.c_str());
    return NULL;
}

void ShaderMgr::release()
{
    // release shaders
    for (auto& pair : shaders)
    {
        // LOG("Free shaders: %s\n", pair.first.c_str() );
        pair.second->release();
    }
}