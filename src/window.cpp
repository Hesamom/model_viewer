#include "window.h"

window::window(int width, int height, const char *windowTitle)
{
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize glfw");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    glfwInit();
    m_Window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);

    if (!m_Window)
    {
        glfwTerminate();
        throw std::runtime_error("failed to create a window");
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    std::cout << glGetString(GL_VERSION) << "Driver OpenGL Version" << std::endl;

    while (isOpen())
    {
        glfwSwapBuffers(m_Window);

        glfwPollEvents();
    }
}

window::~window() {
    glfwTerminate();
}

bool window::isOpen() {
    return !glfwWindowShouldClose(m_Window);
}
