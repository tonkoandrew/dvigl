#include <dvigl/audio.h>

Audio::Audio(std::string filename)
{
    int flags = MIX_INIT_OGG | MIX_INIT_MP3;
    int initted = Mix_Init(flags);
    if ((initted & flags) != flags) {
        LOG("Mix_Init: Failed to init required ogg and mod support!\n");
        LOG("Mix_Init: %s\n", Mix_GetError());
        // handle error
    }
    music = Mix_LoadMUS(filename.c_str());
}

void Audio::play()
{
    if (Mix_PlayingMusic() == 0) {
        Mix_PlayMusic(music, -1);
    }
}

void Audio::pause()
{
    if (Mix_PausedMusic() == 0) {
        Mix_PauseMusic();
    }
}

void Audio::resume()
{
    if (Mix_PausedMusic() != 0) {
        Mix_ResumeMusic();
        // Mix_PlayMusic( music, -1 );
    }
}

void Audio::release() { Mix_FreeMusic(music); }