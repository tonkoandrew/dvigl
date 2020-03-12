#pragma once
#include <dvigl/core.h>

class Texture {
public:
  Texture(SDL_Surface *surf);
  void bind();
  void release();

private:
  GLuint texture_ID;
};
