#pragma once
#include <dvigl/core.h>

class Audio
{
public:
    Audio(std::string filename);
    void release();
    void play();
    void pause();
    void resume();

private:
    Mix_Music* music = NULL;
};