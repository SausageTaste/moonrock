#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>

#include <moonrock/moonrock.hpp>


namespace {

    class WindowSDL {

    public:
        WindowSDL() {
            SDL_Init(SDL_INIT_VIDEO);
            auto flags = SDL_WINDOW_RESIZABLE;
            window_ = SDL_CreateWindow("Moonrock", 1280, 720, flags);
            SDL_ShowWindow(window_);
        }

        ~WindowSDL() {
            if (window_)
                SDL_DestroyWindow(window_);
            window_ = nullptr;
        }

    private:
        SDL_Window* window_ = nullptr;
    };


    class CombinedEngine {

    public:
        CombinedEngine() { system("chcp 65001"); }

        void do_frame() {}

        SDL_AppResult proc_event(const SDL_Event& e) {
            return SDL_AppResult::SDL_APP_CONTINUE;
        }

    private:
        WindowSDL window_;
        moonrock::Renderer renderer_;
    };

}  // namespace


SDL_AppResult SDL_AppInit(void** appstate, int argc, char** argv) {
    *appstate = new ::CombinedEngine();
    return SDL_AppResult::SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void* appstate) {
    auto app = static_cast<::CombinedEngine*>(appstate);
    app->do_frame();
    return SDL_AppResult::SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* e) {
    if (nullptr == e)
        return SDL_AppResult::SDL_APP_CONTINUE;
    if (e->type == SDL_EVENT_QUIT)
        return SDL_AppResult::SDL_APP_SUCCESS;

    auto app = static_cast<::CombinedEngine*>(appstate);
    if (nullptr == app)
        return SDL_AppResult::SDL_APP_FAILURE;

    return app->proc_event(*e);
}


void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    auto app = static_cast<::CombinedEngine*>(appstate);
    delete app;
}
