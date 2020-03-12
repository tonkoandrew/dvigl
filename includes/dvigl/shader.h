#pragma once
#include <dvigl/core.h>

class Shader {
public:
  bool compile_and_link(char *vs_content, char *fs_content);
  void bind();
  void uniformMatrix4(std::string name, glm::mat4 value);
  void uniformMatrix3(std::string name, glm::mat3 value);
  void uniform1i(std::string name, int value);
  void uniform1f(std::string name, float value);
  void uniform3f(std::string name, glm::vec3 value);
  GLint get_uniform_location(std::string name);
  void release();

private:
  GLhandleARB compile(GLenum type, char *content);
  GLhandleARB program_object;
};
