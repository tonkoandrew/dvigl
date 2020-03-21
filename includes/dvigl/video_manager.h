#pragma once
#include <dvigl/core.h>

#include <theoraplay/theoraplay.h>

class VideoMgr : public Singleton<VideoMgr> {
public:
    int quit = 0;
    THEORAPLAY_Decoder *decoder = NULL;
    SDL_Texture *texture = NULL;
    const THEORAPLAY_VideoFrame *video = NULL;
    const THEORAPLAY_AudioPacket *audio = NULL;
    SDL_Window *screen = NULL;
    SDL_AudioSpec spec;
    SDL_Event event;
    Uint32 framems = 0;
    int initfailed = 0;

    void *pixels = NULL;
    int pitch = 0;
    SDL_Renderer* renderer = NULL;

  bool init();
  bool load_video(std::string name);
  void update(float time_delta);
  void release();
};