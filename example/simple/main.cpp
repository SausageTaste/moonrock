#include <cassert>
#include <cmath>
#include <stdexcept>
#include <vector>

#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>

#include <moonrock/moonrock.hpp>


namespace {

    class FramebufferSDL {

    public:
        FramebufferSDL() : tex_(nullptr), width_(0), height_(0) {}

        FramebufferSDL(SDL_Texture* tex, int w, int h)
            : tex_(tex), width_(w), height_(h) {}

        FramebufferSDL(FramebufferSDL&& rhs) noexcept {
            std::swap(tex_, rhs.tex_);
            std::swap(width_, rhs.width_);
            std::swap(height_, rhs.height_);
        }

        FramebufferSDL& operator=(FramebufferSDL&& rhs) noexcept {
            std::swap(tex_, rhs.tex_);
            std::swap(width_, rhs.width_);
            std::swap(height_, rhs.height_);
            return *this;
        }

        ~FramebufferSDL() { this->destroy(); }

        void destroy() {
            if (tex_) {
                SDL_DestroyTexture(tex_);
                tex_ = nullptr;
            }

            width_ = 0;
            height_ = 0;
        }

        SDL_Texture* get() const { return tex_; }

        int width() const { return width_; }
        int height() const { return height_; }

        std::pair<void*, int> lock() {
            void* pixels = NULL;
            int pitch = 0;  // bytes per row
            if (SDL_LockTexture(tex_, NULL, &pixels, &pitch)) {
                return std::make_pair(pixels, pitch);
            }

            SDL_Log("Lock failed: %s", SDL_GetError());
            return std::make_pair(nullptr, 0);
        }

        void unlock() { SDL_UnlockTexture(tex_); }

    private:
        SDL_Texture* tex_;
        int width_;
        int height_;
    };


    class WindowSDL {

    public:
        WindowSDL(int width, int height) {
            SDL_Init(SDL_INIT_VIDEO);
            auto flags = SDL_WINDOW_RESIZABLE;
            window_ = SDL_CreateWindow("Moonrock", width, height, flags);
            renderer_ = SDL_CreateRenderer(window_, nullptr);

            assert(window_ && renderer_);

            SDL_ShowWindow(window_);
        }

        ~WindowSDL() {
            if (renderer_) {
                SDL_DestroyRenderer(renderer_);
                renderer_ = nullptr;
            }

            if (window_) {
                SDL_DestroyWindow(window_);
                window_ = nullptr;
            }
        }

        void present_texture(SDL_Texture* tex) {
            SDL_SetRenderDrawColor(renderer_, 0, 0, 0, 255);
            SDL_RenderClear(renderer_);
            SDL_RenderTexture(renderer_, tex, nullptr, nullptr);
            SDL_RenderPresent(renderer_);
        }

        FramebufferSDL create_fbuf(int width, int height) {
            return FramebufferSDL{
                SDL_CreateTexture(
                    renderer_,
                    SDL_PIXELFORMAT_ARGB8888,
                    SDL_TEXTUREACCESS_STREAMING,
                    width,
                    height
                ),
                width,
                height,
            };
        }

    private:
        SDL_Window* window_ = nullptr;
        SDL_Renderer* renderer_ = nullptr;
    };


    class CombinedEngine {

    public:
        CombinedEngine() : window_(1280, 720) {
            system("chcp 65001");
            fbuf_ = window_.create_fbuf(1280, 720);
        }

        void do_frame() {
            const auto t = SDL_GetTicksNS() / 1'000'000'000.0;  // seconds

            std::vector<uint8_t> fbuf;
            const auto texel_count = fbuf_.width() * fbuf_.height();
            fbuf.resize(texel_count * 4);
            const auto texels = reinterpret_cast<uint32_t*>(fbuf.data());

            for (size_t i = 0; i < texel_count; ++i) {
                const auto x = i % fbuf_.width();
                const auto y = i / fbuf_.width();

                const uint8_t r = x * 255 / fbuf_.width();
                const uint8_t g = y * 255 / fbuf_.height();
                const uint8_t b = std::cos(t) * 127 + 128;
                const uint8_t a = 255;

                texels[i] = (a << 24) | (r << 16) | (g << 8) | b;
            }

            const auto [pixels, pitch] = fbuf_.lock();
            if (pixels == nullptr)
                return;

            const auto row_size = fbuf_.width() * 4;
            for (int y = 0; y < fbuf_.height(); ++y) {
                const auto src_row = &fbuf[y * row_size];
                const auto dst_row = reinterpret_cast<uint8_t*>(pixels) +
                                     y * pitch;
                std::memcpy(dst_row, src_row, row_size);
            }
            fbuf_.unlock();
            window_.present_texture(fbuf_.get());
        }

        SDL_AppResult proc_event(const SDL_Event& e) {
            return SDL_AppResult::SDL_APP_CONTINUE;
        }

    private:
        WindowSDL window_;
        FramebufferSDL fbuf_;
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
