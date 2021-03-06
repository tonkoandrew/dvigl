#pragma once
#include <dvigl/core.h>

class Shader
{
public:
    bool compile_and_link(std::unordered_map<std::string, std::string> source);
    void bind();
    void uniformMatrix4(std::string name, glm::mat4 value);
    void uniformMatrix4(std::string name, std::vector<glm::mat4>* values);
    void uniformMatrix3(std::string name, glm::mat3 value);
    void uniform1i(std::string name, int value);
    void uniform3i(std::string name, glm::ivec3 value);
    void uniform1f(std::string name, float value);
    void uniform1b(std::string name, bool value);
    void uniform3f(std::string name, glm::vec3 value);
    GLint get_uniform_location(std::string name);
    void release();

    bool FindAttribLocation(std::string name);
    GLint attrib(std::string name);

    std::unordered_map<std::string, GLint> attribs_;
private:
    GLuint compile(GLenum type, std::string content);
    GLuint program_object;

};
