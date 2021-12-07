#include <string>
#include <fstream>
#include <stdexcept>

#include <SDL.h>

#include <moonrock/moonrock.h>
#include <moonrock/actor.h>
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

        void flip() {
            SDL_LockSurface(this->m_surface);

            int pitch = this->m_surface->pitch; // row size
            char* temp = new char[pitch]; // intermediate buffer
            char* pixels = (char*) this->m_surface->pixels;

            for(int i = 0; i < this->m_surface->h / 2; ++i) {
                // get pointers to the two rows to swap
                char* row1 = pixels + i * pitch;
                char* row2 = pixels + (this->m_surface->h - i - 1) * pitch;

                // swap rows
                memcpy(temp, row1, pitch);
                memcpy(row1, row2, pitch);
                memcpy(row2, temp, pitch);
            }

            delete[] temp;

            SDL_UnlockSurface(this->m_surface);
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

        moonrock::EulerCamera m_camera;
        moonrock::ModelStatic m_model;
        moonrock::ImageUint2D m_texture;

    public:
        Renderer()
           : m_fbuf(512, 512)
        {
            this->m_camera.pos() = glm::vec3{0, 3, 10};

            const auto resource_path = moonrock::find_parent_folder_containing_folder_named("resource") + "/resource";
            const auto model_buffer = load_from_disk<std::vector<uint8_t>>(resource_path + "/Character Running.dmd");
            if (!moonrock::build_model_from_dmd(model_buffer.data(), model_buffer.size(), this->m_model)) {
                throw std::runtime_error{ "Failed to build model" };
            }

            this->m_texture = load_image_from_disk(resource_path + "/Character Running.png");
        }

        void render() {
            const auto seed = static_cast<float>(moonrock::get_cur_sec());

            const glm::mat4 identity{1};
            const auto model_mat = glm::rotate(identity, glm::radians<float>(seed * 10), glm::vec3{0, 1, 0});
            const auto view_mat = this->m_camera.make_view_mat();
            const auto proj_mat = glm::perspective<float>(glm::radians(90.f), 1.f, 0.1f, 100.f);

            this->m_fbuf.m_color_buffer.fill(moonrock::Pixel4Uint8(0, 0, 0, 1));
            this->m_fbuf.m_depth_map.fill(moonrock::Pixel1Float32{1});
            this->m_shader.draw(
                proj_mat * view_mat * model_mat,
                this->m_model.m_units.front().m_mesh,
                this->m_texture,
                this->m_fbuf
            );
        }

    };

}


int main(int argc, char* args[]) {
    ::WindowSDL window{ "Output Display", 800, 800 };
    ::Renderer renderer;
    ::moonrock::Timer timer;

    while (true) {
        const auto delta_time = timer.check_get_elapsed();
        const auto delta_time_f = static_cast<float>(delta_time);

        SDL_Event e;
        while (::poll_event_sdl(e)) {
            if (SDL_QUIT == e.type) {
                return 0;
            }
        }

        glm::vec3 move_direction{ 0, 0, 0 };
        {
            auto states = SDL_GetKeyboardState(nullptr);

            if (states[SDL_SCANCODE_A])
                move_direction.x -= 1;
            if (states[SDL_SCANCODE_D])
                move_direction.x += 1;
            if (states[SDL_SCANCODE_W])
                move_direction.z -= 1;
            if (states[SDL_SCANCODE_S])
                move_direction.z += 1;

            if (states[SDL_SCANCODE_SPACE])
                move_direction.y += 1;
            if (states[SDL_SCANCODE_LCTRL])
                move_direction.y -= 1;
        }

        constexpr float MOVE_SPEED = 2;
        renderer.m_camera.move_forward(glm::vec3{move_direction.x, 0, move_direction.z} * delta_time_f * MOVE_SPEED);
        renderer.m_camera.pos().y += MOVE_SPEED * move_direction.y * delta_time_f;

        renderer.render();

        ::TextureSDL texture;
        texture.init(
            renderer.m_fbuf.m_color_buffer.data(),
            renderer.m_fbuf.m_color_buffer.width(),
            renderer.m_fbuf.m_color_buffer.height()
        );
        texture.flip();

        SDL_Rect rect{};
        rect.w = window.width();
        rect.h = window.height();
        SDL_BlitScaled(texture.handle(), nullptr, window.surface_handle(), &rect);
        window.update_surface();
    }

    return 0;
}
