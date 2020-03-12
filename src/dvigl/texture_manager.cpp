#include <dvigl/texture.h>
#include <dvigl/texture_manager.h>

TextureMgr gTextureMgr;

bool TextureMgr::init() {
#ifndef __PLATFORM_ANDROID__
  int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG | IMG_INIT_TIF;
  if (!(IMG_Init(imgFlags) & imgFlags)) {
    LOG("SDL_image error: %s\n", IMG_GetError());
    return false;
  }
#endif
  return true;
}

Texture *TextureMgr::get_texture(std::string name) {
  auto it = textures.find(name);
  if (it != textures.end()) {
    return it->second;
  }

  LOG("Texture %s not found\n", name.c_str());
  return NULL;
}

bool TextureMgr::add_texture(std::string name, SDL_Surface *surf) {
  if (!surf) {
    LOG("TextureMgr::add_texture(\"%s\") failed: surf is NULL\n", name.c_str());
    return false;
  }

  SDL_PixelFormat RGBAFormat;
  RGBAFormat.palette = 0;
  RGBAFormat.BitsPerPixel = 32;
  RGBAFormat.BytesPerPixel = 4;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  RGBAFormat.Rmask = 0xFF000000;
  RGBAFormat.Rshift = 0;
  RGBAFormat.Rloss = 0;
  RGBAFormat.Gmask = 0x00FF0000;
  RGBAFormat.Gshift = 8;
  RGBAFormat.Gloss = 0;
  RGBAFormat.Bmask = 0x0000FF00;
  RGBAFormat.Bshift = 16;
  RGBAFormat.Bloss = 0;
  RGBAFormat.Amask = 0x000000FF;
  RGBAFormat.Ashift = 24;
  RGBAFormat.Aloss = 0;
#else
  RGBAFormat.Rmask = 0x000000FF;
  RGBAFormat.Rshift = 24;
  RGBAFormat.Rloss = 0;
  RGBAFormat.Gmask = 0x0000FF00;
  RGBAFormat.Gshift = 16;
  RGBAFormat.Gloss = 0;
  RGBAFormat.Bmask = 0x00FF0000;
  RGBAFormat.Bshift = 8;
  RGBAFormat.Bloss = 0;
  RGBAFormat.Amask = 0xFF000000;
  RGBAFormat.Ashift = 0;
  RGBAFormat.Aloss = 0;
#endif
  SDL_Surface *converted = NULL;

  converted = SDL_ConvertSurface(surf, &RGBAFormat, SDL_SWSURFACE);

  if (!converted) {
    LOG("SDL_ConvertSurface error: %s\n", SDL_GetError());
    return false;
  }

  Texture *texture = new Texture(converted);
  textures[name] = texture;
  return true;
}

bool TextureMgr::load_texture(std::string name, std::string file_name) {
  SDL_Surface *surf = IMG_Load(file_name.c_str());
  if (!surf) {
    LOG("IMG_Load error: %s\n", IMG_GetError());
    return false;
  }

  SDL_PixelFormat RGBAFormat;
  RGBAFormat.palette = 0;
  RGBAFormat.BitsPerPixel = 32;
  RGBAFormat.BytesPerPixel = 4;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  RGBAFormat.Rmask = 0xFF000000;
  RGBAFormat.Rshift = 0;
  RGBAFormat.Rloss = 0;
  RGBAFormat.Gmask = 0x00FF0000;
  RGBAFormat.Gshift = 8;
  RGBAFormat.Gloss = 0;
  RGBAFormat.Bmask = 0x0000FF00;
  RGBAFormat.Bshift = 16;
  RGBAFormat.Bloss = 0;
  RGBAFormat.Amask = 0x000000FF;
  RGBAFormat.Ashift = 24;
  RGBAFormat.Aloss = 0;
#else
  RGBAFormat.Rmask = 0x000000FF;
  RGBAFormat.Rshift = 24;
  RGBAFormat.Rloss = 0;
  RGBAFormat.Gmask = 0x0000FF00;
  RGBAFormat.Gshift = 16;
  RGBAFormat.Gloss = 0;
  RGBAFormat.Bmask = 0x00FF0000;
  RGBAFormat.Bshift = 8;
  RGBAFormat.Bloss = 0;
  RGBAFormat.Amask = 0xFF000000;
  RGBAFormat.Ashift = 0;
  RGBAFormat.Aloss = 0;
#endif
  SDL_Surface *converted = NULL;

  converted = SDL_ConvertSurface(surf, &RGBAFormat, SDL_SWSURFACE);

  if (!converted) {
    LOG("SDL_ConvertSurface error: %s\n", SDL_GetError());
    return false;
  }

  Texture *texture = new Texture(converted);
  textures[name] = texture;
  return true;
}

void TextureMgr::release() {
  // release textures
  IMG_Quit();
}
