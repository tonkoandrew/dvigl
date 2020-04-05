#pragma once
#include <dvigl/core.h>

class Texture;

class TextureMgr : public Singleton<TextureMgr>
{
public:
    bool init();
    Texture* get_texture(std::string name);
    bool load_texture(std::string name, std::string file_name);
    bool add_texture(std::string name, SDL_Surface* surf);
    void release();

private:
    std::map<std::string, Texture*> textures;
};