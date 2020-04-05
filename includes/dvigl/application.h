#pragma once
#include <dvigl/core.h>

class Application : public Singleton<Application>
{
public:
    bool init();
    bool main_loop();
    void exit();
    int get_error_code();
    void release();

private:
    bool quit;
    int error_code;
};
