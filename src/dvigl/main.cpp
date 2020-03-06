#include <dvigl/core.h>
#include <dvigl/application.h>

#ifdef __PLATFORM_ANDROID__
int SDL_main(int argc, char * argv[])
#else
#undef main
int main(int argc, char * argv[])
#endif
{
    LOG("Hello\n");
    Application * app = Application::ptr();
    if (app->init(argc, argv))
    {
        if (app->main_loop())
        {
            app->release();
            LOG("Exiting\n");
            std::exit(0);
        }
    }
    return app->get_error_code();
}