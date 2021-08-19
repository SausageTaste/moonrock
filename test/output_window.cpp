#include <chrono>
#include <thread>
#include <future>
#include <cassert>
#include <fstream>
#include <iostream>
#include <exception>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

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

    moonrock::ImageUint2D load_image_from_disk(const std::string& path) {
        const auto buffer = load_from_disk<std::vector<uint8_t>>(path);
        return moonrock::parse_image_from_memory(buffer.data(), buffer.size());
    }

    std::string load_str_from_disk(const std::string& path) {
        return ::load_from_disk<std::string>(path);
    }

}


// GLFW
namespace {

    constexpr unsigned int MIN_WIN_WIDTH = 128;
    constexpr unsigned int MIN_WIN_HEIGHT = 128;


    void callback_error(int error, const char* description) {
        std::cout << description << std::endl;
    }

    void callback_resize_fbuf(GLFWwindow* const window, int width, int height) {
        glViewport(0, 0, width, height);
    }


    class WindowGLFW {

    private:
        GLFWwindow* m_window = nullptr;

    public:
        WindowGLFW(const char* const title, int winWidth, int winHeight, bool fullscreen) {
            glfwSetErrorCallback(callback_error);

            if (GLFW_FALSE == glfwInit())
                throw std::runtime_error("failed to initialize GLFW");

            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
            this->m_window = glfwCreateWindow(winWidth, winHeight, title, NULL, NULL);
            if (nullptr == this->m_window)
                throw std::runtime_error("failed to create window");

            glfwSetFramebufferSizeCallback(this->m_window, ::callback_resize_fbuf);

            glfwSetWindowSizeLimits(this->m_window, MIN_WIN_WIDTH, MIN_WIN_HEIGHT, GLFW_DONT_CARE, GLFW_DONT_CARE);
            glfwMakeContextCurrent(this->m_window);
            if (0 == gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                throw std::runtime_error("failed to load OpenGL functions");
            }
            glfwSwapInterval(1);
        }

        ~WindowGLFW(void) {
            glfwDestroyWindow(this->m_window);
            this->m_window = nullptr;
            glfwTerminate();
        }

        void poll_events() {
            glfwPollEvents();
        }

        void swap_buffers(void) {
            glfwSwapBuffers(this->m_window);
        }

        bool should_close(void) {
            return glfwWindowShouldClose(this->m_window);
        }

        auto get_window_size(void) {
            int w, h;
            glfwGetFramebufferSize(this->m_window, &w, &h);
            return std::make_pair(w, h);
        }

        auto get_mouse_pos(void) {
            double xpos, ypos;
            glfwGetCursorPos(this->m_window, &xpos, &ypos);
            return std::make_pair(xpos, ypos);
        }

    };

}


// OpenGL
namespace {

    enum class ShaderType{ vertex, fragment };


    class ShaderRAII {

    private:
        GLuint m_id;

    public:
        ShaderRAII(const ::ShaderType type, const char* const src) {
            this->init(type, src);
        }

        ~ShaderRAII(void) {
            this->destroy();
        }

        void init(const ::ShaderType type, const char* const src) {
            this->destroy();

            switch (type) {
                case ::ShaderType::vertex:
                    this->m_id = glCreateShader(GL_VERTEX_SHADER);
                    break;
                case ::ShaderType::fragment:
                    this->m_id = glCreateShader(GL_FRAGMENT_SHADER);
                    break;
            }

            assert(0 != this->m_id);

            glShaderSource(this->m_id, 1, &src, nullptr);
            glCompileShader(this->m_id);

            GLint shader_compiled = GL_FALSE;
            glGetShaderiv(this->m_id, GL_COMPILE_STATUS, &shader_compiled);
            if (GL_TRUE != shader_compiled) {
                constexpr auto SHADER_COMPILER_LOG_BUF_SIZE = 2048;
                GLsizei length = 0;
                char log[SHADER_COMPILER_LOG_BUF_SIZE];
                glGetShaderInfoLog(this->m_id, SHADER_COMPILER_LOG_BUF_SIZE, &length, log);
                throw std::runtime_error(log);
            }
        }

        void destroy() {
            if (this->is_ready()) {
                glDeleteShader(this->m_id);
                this->m_id = 0;
            }
        }

        bool is_ready() const {
            return 0 != this->m_id;
        }

        GLuint get() const {
            return this->m_id;
        }

    };


    class ShaderProgram {

    private:
        GLuint m_program = 0;

    public:
        ShaderProgram() = default;

        ShaderProgram(const char* const vert_src, const char* const frag_src) {
            this->init(vert_src, frag_src);
        }

        ~ShaderProgram() {
            this->destroy();
        }

        void init(const char* const vert_src, const char* const frag_src) {
            this->destroy();
            this->m_program = glCreateProgram();
            assert(0 != this->m_program);

            const ::ShaderRAII vert_shader(::ShaderType::vertex, vert_src);
            const ::ShaderRAII frag_shader(::ShaderType::fragment, frag_src);

            glAttachShader(this->m_program, vert_shader.get());
            glAttachShader(this->m_program, frag_shader.get());
            glLinkProgram(this->m_program);

            GLint success = GL_TRUE;
            glGetProgramiv(this->m_program, GL_LINK_STATUS, &success);
            if (GL_TRUE != success) {
                GLsizei length = 0;
                char log[100];
                glGetProgramInfoLog(this->m_program, 100, &length, log);
                throw std::runtime_error(log);
            }
        }

        void destroy() {
            if (this->is_ready()) {
                glDeleteProgram(this->m_program);
                this->m_program = 0;
            }
        }

        bool is_ready() const {
            return 0 != this->m_program;
        }

        void use() const {
            glUseProgram(this->m_program);
        }

    };


    class TextureGL {

    private:
        GLuint m_tex = 0;

    public:
        void init() {
            this->destroy();

            glGenTextures(1, &this->m_tex);
            assert(0 != this->m_tex);
            glBindTexture(GL_TEXTURE_2D, this->m_tex);
//*/
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
/*/
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//*/
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        void destroy() {
            if (this->is_ready()) {
                glDeleteTextures(1, &this->m_tex);
                this->m_tex = 0;
            }
        }

        void update(const moonrock::ImageUint2D& image) {
            assert(this->is_ready());

            glBindTexture(GL_TEXTURE_2D, this->m_tex);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        bool is_ready() const {
            return 0 != this->m_tex;
        }

        auto get() const {
            return this->m_tex;
        }

    };

}


namespace {

    class RenderMaster {

    private:
        ::WindowGLFW m_window;
        ::ShaderProgram m_shader;
        ::TextureGL m_texture;

    public:
        RenderMaster()
            : m_window("Moonrock renderer", 800, 800, false)
        {
            const auto resource_folder = moonrock::find_parent_folder_containing_folder_named("resource") + "/resource";

            this->m_shader.init(
                ::load_str_from_disk(resource_folder + "/fill_screen.vert").c_str(),
                ::load_str_from_disk(resource_folder + "/fill_screen.frag").c_str()
            );

            this->m_texture.init();

            glClearColor(0, 0, 0, 1);
        }

        bool update() {
            this->m_window.poll_events();

            glClear(GL_COLOR_BUFFER_BIT);

            if (this->m_texture.is_ready()) {
                this->m_shader.use();
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, this->m_texture.get());
                glDrawArrays(GL_TRIANGLES, 0, 3);
            }

            this->m_window.swap_buffers();
            glFlush();
            return !this->m_window.should_close();
        }

        void set_image(const moonrock::ImageUint2D& image) {
            this->m_texture.update(image);
        }

    };

}


int main() {
    ::RenderMaster renderer;

    moonrock::Timer timer;
    moonrock::Framebuffer fbuf{ 1024, 1024 };
    moonrock::VertexBuffer<moonrock::VertexStatic> vbuf;
    moonrock::Shader shader;

    const auto resource_path = moonrock::find_parent_folder_containing_folder_named("resource") + "/resource";
    const auto model_buffer = ::load_from_disk<std::vector<uint8_t>>(resource_path + "/Character Running.dmd");
    const auto model_data = moonrock::build_model_from_dmd(model_buffer.data(), model_buffer.size());
    const auto texture = ::load_image_from_disk("C:\\Users\\woos8\\Downloads\\albedo_map.jpg");

    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{-1, -1, 0}, glm::vec3{-1, 1, 0}, glm::vec3{1, 1, 0}, glm::vec3{1, -1, 0});
    moonrock::gen_mesh_quad(vbuf.m_vertices, glm::vec3{0, -1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, -1}, glm::vec3{0, 1 + 0.3, 1}, glm::vec3{0, -1 + 0.3, 1});

    auto render = [&]() {
        fbuf.m_color_buffer.fill(moonrock::Pixel4Uint8(0, 0, 0, 1));
        fbuf.m_depth_map.fill(moonrock::Pixel1Float32{1});
        shader.draw(model_data->m_units.front().m_mesh, texture, fbuf);
    };

    do {
        if (timer.elapsed() > 1) {
            render();
            renderer.set_image(fbuf.m_color_buffer);
            timer.check();
        }

    } while (renderer.update());

    return 0;
}
