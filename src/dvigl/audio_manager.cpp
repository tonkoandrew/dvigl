#include <dvigl/audio.h>
#include <dvigl/audio_manager.h>

AudioMgr gAudioMgr;

bool AudioMgr::init()
{
    if (Mix_Init(MIX_INIT_OGG) == -1) {
        LOG("Mix init error: %s\n", Mix_GetError());
        return false;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        return false;
    }
    SDL_ClearError();
    return true;
}

void AudioMgr::release()
{
    // release audio buffers
    Mix_CloseAudio();
    Mix_Quit();
}

bool AudioMgr::load_audio(std::string name, std::string filename)
{
    Audio* a = new Audio(filename);
    if (!a) {
        return false;
    }
    audios[name] = a;
    return true;
}

Audio* AudioMgr::get_audio(std::string name)
{
    auto it = audios.find(name);
    if (it != audios.end()) {
        return it->second;
    }

    LOG("Audio %s not found\n", name.c_str());
    return NULL;
}

void AudioMgr::set_volume(int vol)
{
    volume = vol;
    Mix_VolumeMusic(volume);
}
