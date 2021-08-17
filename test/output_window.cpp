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


int main() {
    WindowGLFW window{ "Moonrock renderer", 512, 512, false };

    while (!window.should_close()) {
        window.poll_events();

        glClearColor(0.2, 0, 0.2, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        window.swap_buffers();
    }

    return 0;
}
