#include <string>
#include <fstream>
#include <stdexcept>

#include <SDL.h>

#include <moonrock/moonrock.h>
#include <moonrock/utils.h>


namespace {

    template <typename _Container>
    _Container load_from_disk(const char* const path) {
        std::fstream file;
        file.open(path, std::ios::in | std::ios::ate | std::ios::binary);
        if (!file.is_open())
            throw std::runtime_error{"failed to open file"};

        const auto content_size = file.tellg();
        file.seekg(0);

        _Container buffer;
        buffer.resize(content_size);
        file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());

        return buffer;
    }

    template <typename _Container>
    _Container load_from_disk(const std::string path) {
        return load_from_disk<_Container>(path.c_str());
    }

    std::string load_str_from_disk(const std::string& path) {
        return ::load_from_disk<std::string>(path);
    }

}


// SDL
namespace {

    bool poll_event_sdl(SDL_Event& e) {
        return 0 != SDL_PollEvent(&e);
    }


    class TextureSDL {

    private:
        SDL_Surface* m_surface = nullptr;

    public:
        ~TextureSDL() {
            this->destroy();
        }

        bool init_bmp(const char* const img_file_path) {
            this->m_surface = SDL_LoadBMP(img_file_path);
            if (nullptr != this->m_surface) {
                return false;
            }

            return true;
        }

        bool init(
            void* const pixels,
            const unsigned width,
            const unsigned height
        ) {
            Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#else
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#endif

            const int depth = 32;
            const int pitch = 4 * width;

            this->m_surface = SDL_CreateRGBSurfaceFrom(
                pixels, width, height, depth, pitch, rmask, gmask, bmask, amask
            );

            if (nullptr != this->m_surface) {
                return false;
            }

            return true;
        }

        void destroy() {
            if (nullptr != this->m_surface) {
                SDL_FreeSurface(this->m_surface);
                this->m_surface = nullptr;
            }
        }

        auto handle() const {
            return this->m_surface;
        }

    };


    class WindowSDL {

    private:
        SDL_Window* m_window = nullptr;
        SDL_Surface* m_surface = nullptr;
        unsigned m_window_width;
        unsigned m_window_height;

    public:
        WindowSDL(
            const char* const title,
            const unsigned window_width,
            const unsigned window_height
        ) {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                throw std::runtime_error{"Failed to initiate SDL"};
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
                throw std::runtime_error{"Failed to create SDL window"};
            }

            this->m_surface = SDL_GetWindowSurface(this->m_window);

            this->m_window_width = window_width;
            this->m_window_height = window_height;
        }

        ~WindowSDL() {
            if (nullptr != this->m_window) {
                SDL_DestroyWindow(this->m_window);
                SDL_Quit();
            }

            this->m_window = nullptr;
            this->m_surface = nullptr;
        }

        void update_surface() {
            SDL_UpdateWindowSurface(this->m_window);
        }

        auto window_handle() const {
            return this->m_window;
        }

        auto surface_handle() const {
            return this->m_surface;
        }

        auto width() const {
            return this->m_window_width;
        }

        auto height() const {
            return this->m_window_height;
        }

    };

}


// moonrock
namespace {

    moonrock::ImageUint2D load_image_from_disk(const std::string& path) {
        const auto buffer = load_from_disk<std::vector<uint8_t>>(path);
        return moonrock::parse_image_from_memory(buffer.data(), buffer.size());
    }


    class Renderer {

    public:
        moonrock::Framebuffer m_fbuf;
        moonrock::Shader m_shader;

        moonrock::ModelStatic m_model;
        moonrock::ImageUint2D m_texture;

    public:
        Renderer()
           : m_fbuf(512, 512)
        {
            const auto resource_path = moonrock::find_parent_folder_containing_folder_named("resource") + "/resource";
            const auto model_buffer = load_from_disk<std::vector<uint8_t>>(resource_path + "/Character Running.dmd");
            if (!moonrock::build_model_from_dmd(model_buffer.data(), model_buffer.size(), this->m_model)) {
                throw std::runtime_error{ "Failed to build model" };
            }

            this->m_texture = load_image_from_disk(resource_path + "/Character Running.png");
        }

        void render() {
            this->m_fbuf.m_color_buffer.fill(moonrock::Pixel4Uint8(0, 0, 0, 1));
            this->m_fbuf.m_depth_map.fill(moonrock::Pixel1Float32{1});
            this->m_shader.draw(this->m_model.m_units.front().m_mesh, this->m_texture, this->m_fbuf);
        }

    };

}


int main(int argc, char* args[]) {
    ::WindowSDL window{ "Output Display", 800, 800 };

    while (true) {
        SDL_Event e;
        while (::poll_event_sdl(e)) {
            if (SDL_QUIT == e.type) {
                return 0;
            }
        }

        ::Renderer renderer;
        renderer.render();

        ::TextureSDL texture;
        texture.init(
            renderer.m_fbuf.m_color_buffer.data(),
            renderer.m_fbuf.m_color_buffer.width(),
            renderer.m_fbuf.m_color_buffer.height()
        );

        SDL_Rect rect{};
        rect.w = window.width();
        rect.h = window.height();
        SDL_BlitScaled(texture.handle(), nullptr, window.surface_handle(), &rect);
        window.update_surface();
    }

    return 0;
}
