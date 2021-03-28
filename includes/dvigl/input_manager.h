#pragma once
#include <dvigl/core.h>

class InputMgr : public Singleton<InputMgr>
{
public:
    bool init();
    void process_input(float time_delta);
    void release();
    void mouse_motion(SDL_MouseMotionEvent* event);

private:
    const Uint8* keystates;
};