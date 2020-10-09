#include <dvigl/application.h>
#include <dvigl/core.h>

#include <dvigl/render_manager.h>

extern "C" {
    double __exp_finite(double x) { return exp(x); }
    double __log_finite(double x) { return log(x); }
    double __pow_finite(double x, double y) { return pow(x, y); }

    float __expf_finite(float x) { return expf(x); }
    float __logf_finite(float x) { return logf(x); }
    float __powf_finite(float x, float y) { return powf(x, y); }
    float __acosf_finite(float x) { return acosf(x); }

    int dvigl_init()
    {
        Application* app = Application::ptr();
        if (app->init())
        {
            return 0;
        }
        return app->get_error_code();
    }

    int dvigl_main()
    {
        Application* app = Application::ptr();
        if (app->main_loop())
        {
            app->release();
            return 0;
        }
        return app->get_error_code();
    }

    Uint32 dvigl_get_window_id()
    {
        SDL_Window* w = RenderMgr::ptr()->get_main_window();
        return SDL_GetWindowID(w);
    }

}