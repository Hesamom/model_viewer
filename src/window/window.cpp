#include <GL/glew.h>
#include "window.h"



void window::onSizeChanged(int width, int height)
{
    glViewport(0, 0, width, height);
    m_Width = width;
    m_Height = height;
}

window::window(int width, int height, const std::string& title, bool fullscreen) 
{
    if (title.empty())
    {
        throw std::runtime_error("title can not be empty");
    }
    
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize glfw");
    }

    //TODO need to learn more about these versions and compact profile 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
    
    if (fullscreen)
    {
        m_Monitor = glfwGetPrimaryMonitor();
    }
    
    m_Window = glfwCreateWindow(width, height, title.c_str(), m_Monitor, nullptr);
    glViewport(0, 0, width, height);
   
    
    //auto callback = [this](GLFWwindow* window, int width, int height){onSizeChanged(width,height);};
   // std::function function = callback;
    //glfwSetWindowSizeCallback(m_Window, callback);
    
    m_Width = width;
    m_Height = height;
    m_Title = title;

    if (!m_Window)
    {
        glfwTerminate();
        throw std::runtime_error("failed to create a window");
    }

    initContext();
}


window::~window() {
    glfwDestroyWindow(m_Window);
}

bool window::isOpen() {
    //TODO implement this 
    return true;
}

void window::draw() 
{
    while (!glfwWindowShouldClose(m_Window))
    {
        glfwSwapBuffers(m_Window);
        onRender();
        glfwPollEvents();
    }
}

void window::onRender() {

}

void window::setSize(int width, int height) {
    
    if (width <= 0)
    {
        throw std::runtime_error("width is less or equal to zero");
    }

    if (height <= 0)
    {
        throw std::runtime_error("height is less or equal to zero");
    }

    glfwSetWindowSize(m_Window, width, height);
    m_Height = height;
    m_Width = width;
}

int window::getHeight() {
    return m_Height;
}

int window::getWidth() {
    return m_Width;
}

void window::initContext() {
    
    glfwMakeContextCurrent(m_Window);

    //TODO need to learn more about the interval and its connection to swap chains 
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE; // Enable experimental features
    glewInit();
    
    std::cout<< "window with title: \"" << m_Title << "\" was created successfully \n";
    std::cout << glGetString(GL_VERSION) << " OpenGL Driver Version \n";
}

void window::setTitle(std::string title) {
    glfwSetWindowTitle(m_Window, title.c_str());
    m_Title = title;
}

std::string window::getTitle() {
    return m_Title;
}

void window::onInit() {

}

