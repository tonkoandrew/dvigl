#include <dvigl/font.h>


Font::Font(std::string filename, int size)
{
    f = TTF_OpenFont(filename.c_str(), size);
    if (!f)
    {
        LOG("TTF_OpenFont error: %s\n", TTF_GetError());
    }
}

SDL_Surface * Font::printf_get_surface(std::string text, SDL_Color * color)
{
    SDL_Surface * surf = TTF_RenderUTF8_Blended(f, text.c_str(), *color);
    if (!surf)
    {
        LOG("TTF_RenderUTF8_Blended error: %s\n", TTF_GetError());
        return NULL;
    }
    return surf;
}

Font::~Font() { TTF_CloseFont(f); }
