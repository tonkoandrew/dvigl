#include <dvigl/font_manager.h>
#include <dvigl/font.h>

FontMgr gFontMgr;

bool FontMgr::init()
{
    if (TTF_Init() == -1)
    {
        LOG("TTF_Init error: %s\n", TTF_GetError());
        return false;
    }
    return true;
}

bool FontMgr::load_font(std::string name, std::string file_name, int size)
{
    Font * font = new Font(file_name, size);
    if (!font->f)
    {
        return false;
    }
    fonts[name] = font;
    // LOG("Add font %s\n", name.c_str());
    return true;
}

Font * FontMgr::get_font(std::string name)
{
    auto it = fonts.find(name);
    if (it == fonts.end())
    {
        LOG("Font %s not found\n", name.c_str());
        return NULL;
    }
    return fonts[name];
}

void FontMgr::release()
{
    // release ttf fonts
    for (auto & pair : fonts)
    {
        // LOG("Free font %s\n", pair.first.c_str() );
        delete pair.second;
    }
    TTF_Quit();
}
