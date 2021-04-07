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
    entt::registry m_Registry;

    bool quit;

private:
    int error_code;
};
