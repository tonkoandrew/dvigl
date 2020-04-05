#pragma once
#include <dvigl/core.h>

class Font
{
public:
    Font(std::string filename, int size);
    ~Font();
    TTF_Font* f = NULL;
    SDL_Surface* printf_get_surface(std::string text, SDL_Color* color);
};
