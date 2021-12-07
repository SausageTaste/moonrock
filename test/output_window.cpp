#include <SDL.h>


namespace {

    class WindowSDL {

    private:
        SDL_Window* m_window = nullptr;
        SDL_Surface* m_surface = nullptr;

    public:
        ~WindowSDL() {
            this->destroy();
        }

        bool init(
            const char* const title,
            const unsigned window_width,
            const unsigned window_height
        ) {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                return false;
            }

            this->m_window = SDL_CreateWindow(
                title,
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                static_cast<int>(window_width),
                static_cast<int>(window_height),
                SDL_WINDOW_SHOWN
            );
            if (nullptr == this->m_window) {
                return false;
            }

            this->m_surface = SDL_GetWindowSurface(this->m_window);

            return true;
        }

        void destroy() {
            if (nullptr != this->m_window) {
                SDL_DestroyWindow(this->m_window);
                SDL_Quit();

                this->m_window = nullptr;
                this->m_surface = nullptr;
            }
        }

        void update_surface() {
            SDL_UpdateWindowSurface(this->m_window);
        }

    };

}


int main(int argc, char* args[]) {
    ::WindowSDL window;
    window.init("Output Display", 800, 450);

    SDL_Delay( 2000 );

    return 0;
}
