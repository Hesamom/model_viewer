#include "GLFW/glfw3.h"
#include "window_glfw.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

window_glfw::window_glfw(int width, int height, const std::string& title, bool fullscreen, bool vSync,  int mssaLevel ) 
{
    if (title.empty())
    {
        throw std::runtime_error("title can not be empty");
    }
    
    if (!glfwInit())
    {
        throw std::runtime_error("failed to initialize glfw");
    }

    if (fullscreen)
    {
        m_Monitor = glfwGetPrimaryMonitor();
    }
    
    //BUG fullscreen mode casues exception 
    m_Window = glfwCreateWindow(width, height, title.c_str(), m_Monitor, nullptr);
    glViewport(0, 0, width, height);
    
    m_Width = width;
    m_Height = height;
    m_Title = title;

    if (!m_Window)
    {
        glfwTerminate();
        throw std::runtime_error("failed to create a window");
    }

	std::cout<< "window with title: \"" << m_Title << "\" was created successfully \n";
	
    subscribeEvents();
}


window_glfw::~window_glfw() {
	
    /*ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();*/
	
    glfwDestroyWindow(m_Window);
}

bool window_glfw::isOpen() {
    //TODO implement this 
    return true;
}


void window_glfw::setSize(int width, int height) {
    
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

int window_glfw::getHeight() {
    glfwGetWindowSize(m_Window, &m_Width, &m_Height);
    return m_Height;
}

int window_glfw::getWidth() {
     glfwGetWindowSize(m_Window, &m_Width, &m_Height);
     return m_Width;
}


void window_glfw::setTitle(std::string title) {
    glfwSetWindowTitle(m_Window, title.c_str());
    m_Title = title;
}

std::string window_glfw::getTitle() {
    return m_Title;
}


/*void window_glfw::setVsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}*/

void window_glfw::onScrollChanged(double yOffset) {

}

void window_glfw::subscribeEvents() {
    
    static auto callback_static = [this](GLFWwindow* window, double xoffset, double yoffset){
        onScrollChanged(yoffset);
    };
    
    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
                       {
                           callback_static(window, xOffset, yOffset);
                       }
    );

    static auto callback_static_2 = [this](GLFWwindow* window, int button, int action, int mods){
        onMouseButtonChanged(button, action, mods);
    };

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
                          {
                              callback_static_2(window, button, action, mods);
                          }
    );

    static auto callback_static_3 = [this](GLFWwindow* window, double xpos, double ypos){
        onMousePositionChanged(xpos, ypos);
    };

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
                               {
                                   callback_static_3(window, xpos, ypos);
                               }
    );

    static auto callback_static_4 = [this](GLFWwindow* window, int width, int height){
        onSizeChanged(height, width);
    };

    glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
                               {
                                   callback_static_4(window, width, height);
                               }
    );
}

void window_glfw::onMouseButtonChanged(int button, int action, int mods) {
    

}

void window_glfw::onMousePositionChanged(double xpos, double ypos) {
    
}

void window_glfw::onSizeChanged(int height, int width) {
    m_Height = height;
    m_Width = width;

    m_Paused = m_Height == 0 || m_Width == 0;
	
	m_SizeCallback(width, height);
}

void window_glfw::pollEvents()
{
	glfwPollEvents();
}

bool window_glfw::isPaused()
{
	return m_Paused;
}

bool window_glfw::shouldClose()
{
	return glfwWindowShouldClose(m_Window);
}

void window_glfw::setGlContext(int major, int minor, int mssa)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, mssa);
	
	glfwMakeContextCurrent(m_Window);
}

void window_glfw::swapBuffers()
{
	glfwSwapBuffers(m_Window);
}

void window_glfw::setOnSizeChangedCallback(std::function<void(int, int)> callback)
{
	m_SizeCallback = callback;
}

