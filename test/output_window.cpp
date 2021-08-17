#include <cassert>
#include <iostream>
#include <exception>

#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>


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
            glfwSwapInterval(0);
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


namespace {

    class ShaderRAII {

    private:
        GLuint m_id;

    public:
        explicit
        ShaderRAII(const GLuint shader)
            : m_id(shader)
        {

        }

        ~ShaderRAII(void) {
            glDeleteShader(this->m_id);
            this->m_id = 0;
        }

        GLuint get() const {
            return this->m_id;
        }

    };


    enum class ShaderType{ vertex, fragment };

    ShaderRAII compile_shader(const ::ShaderType type, const char* const src) {
        // Returns 0 on error
        GLuint shader_id = 0;

        switch (type) {
            case ::ShaderType::vertex:
                shader_id = glCreateShader(GL_VERTEX_SHADER);
                break;
            case ::ShaderType::fragment:
                shader_id = glCreateShader(GL_FRAGMENT_SHADER);
                break;
        }

        assert(0 != shader_id);

        glShaderSource(shader_id, 1, &src, nullptr);
        glCompileShader(shader_id);

        GLint shader_compiled = GL_FALSE;
        glGetShaderiv(shader_id, GL_COMPILE_STATUS, &shader_compiled);
        if (GL_TRUE != shader_compiled) {
            constexpr auto SHADER_COMPILER_LOG_BUF_SIZE = 2048;
            GLsizei length = 0;
            char log[SHADER_COMPILER_LOG_BUF_SIZE];
            glGetShaderInfoLog(shader_id, SHADER_COMPILER_LOG_BUF_SIZE, &length, log);
            throw std::runtime_error(log);
        }

        return ShaderRAII{ shader_id };
    }


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

            const auto vert_shader = ::compile_shader(ShaderType::vertex, vert_src);
            const auto frag_shader = ::compile_shader(ShaderType::fragment, frag_src);

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

}


int main() {
    WindowGLFW window{ "Moonrock renderer", 512, 512, false };

    ::ShaderProgram shader(
        "#version 330 core\n"
        "\n"
        "vec2 POSITIONS[3] = vec2[](\n"
        "    vec2(-1.0,  1.0),\n"
        "    vec2(-1.0, -3.0),\n"
        "    vec2( 3.0,  1.0)\n"
        ");\n"
        "\n"
        "void main() {\n"
        "    gl_Position = vec4(POSITIONS[gl_VertexID], 0.0, 1.0);\n"
        "}\n"
        ,
        "#version 330 core\n"
        "\n"
        "out vec4 f_color;\n"
        "\n"
        "void main() {\n"
        "    f_color = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n"
    );

    while (!window.should_close()) {
        window.poll_events();

        glClearColor(0.2, 0, 0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.swap_buffers();
    }

    return 0;
}
