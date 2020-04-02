#pragma once
#include <dvigl/core.h>

class Font;

class FontMgr : public Singleton<FontMgr> {
public:
    bool init();
    Font* get_font(std::string name);
    bool load_font(std::string name, std::string file_name, int size);
    void release();

private:
    std::map<std::string, Font*> fonts;
};