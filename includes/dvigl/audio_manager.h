#pragma once
#include <dvigl/core.h>

class Audio;

class AudioMgr : public Singleton<AudioMgr>
{
public:
    bool init();
    bool load_audio(std::string name, std::string filename);
    Audio* get_audio(std::string name);
    void release();
    void set_volume(int vol);

private:
    int volume;
    std::unordered_map<std::string, Audio*> audios;
};