#pragma once
#include <dvigl/core.h>

class Shader
{
public:
    bool compile_and_link(std::string vs_content, std::string fs_content);
    void bind();
    void uniformMatrix4(std::string name, glm::mat4 value);
    void uniformMatrix4(std::string name, std::vector<glm::mat4>* values);
    void uniformMatrix3(std::string name, glm::mat3 value);
    void uniform1i(std::string name, int value);
    void uniform1f(std::string name, float value);
    void uniform1b(std::string name, bool value);
    void uniform3f(std::string name, glm::vec3 value);
    GLint get_uniform_location(std::string name);
    void release();

private:
    GLuint compile(GLenum type, std::string content);
    GLuint program_object;
};
