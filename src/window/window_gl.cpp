#include <GL/glew.h>
#include "GLFW/glfw3.h"
#include "window_gl.h"
#include "../common/stopwatch.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

window_gl::window_gl(int width, int height, const std::string& title, bool fullscreen, bool vSync,  int mssaLevel ) 
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
    glfwWindowHint(GLFW_SAMPLES, mssaLevel);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);


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

    subscribeEvents();
    initContext(vSync);
}


window_gl::~window_gl() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(m_Window);
}

bool window_gl::isOpen() {
    //TODO implement this 
    return true;
}

void capTargetFrameRate(double elapsed, int targetFps)
{
    if(targetFps == -1)
    {
        return;
    }

    int fps = 1/elapsed;
    if (fps > targetFps)
    {
        double targetElapsedTime = 1.0/targetFps ;
        double remaining = (targetElapsedTime - elapsed);
        stopwatch stopwatch;
        stopwatch.start();
        do{
            stopwatch.stop();
        }
        while(stopwatch.getSeconds() < remaining);

        stopwatch.stop();
    }
}

void window_gl::draw()
{
    stopwatch watch;
    watch.start();
    while (!glfwWindowShouldClose(m_Window))
    {
        if (m_Paused)
        {
            glfwPollEvents();
            continue;
        }
        
        watch.stop();
        double elapsed = watch.getSeconds();
        watch.start();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        onRenderImGUI();
        //ImGui::ShowDemoWindow();
        
        glfwPollEvents();
        onRender((float)elapsed);
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(m_Window);
        watch.stop();
        m_elapsedTimeSinceStart += watch.getSeconds();
        capTargetFrameRate(watch.getSeconds(), m_TargetFrameRate);
    }
}

void window_gl::onRender(float elapsedTime) {

}

void window_gl::setSize(int width, int height) {
    
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

int window_gl::getHeight() {
    glfwGetWindowSize(m_Window, &m_Width, &m_Height);
    return m_Height;
}

int window_gl::getWidth() {
     glfwGetWindowSize(m_Window, &m_Width, &m_Height);
     return m_Width;
}


void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cerr << "OpenGL Debug Message: " << message << std::endl;
            break;
    }
}

void window_gl::initContext(bool vSync) {
    
    glfwMakeContextCurrent(m_Window);
    glewExperimental = GL_TRUE; // Enable experimental features
    glewInit();
    glfwSwapInterval(vSync ? 1 : 0);
    
    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);

    glEnable(GL_DEPTH_TEST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

    if (GLEW_KHR_debug) {
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        
        glDebugMessageCallback(debugCallback, nullptr);
        // Enable only high-severity error messages
        glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_ERROR, GL_DEBUG_SEVERITY_HIGH, 0, nullptr, GL_TRUE);
    } 
    else 
    {
        std::cerr << "KHR_debug extension not supported" << std::endl;
    }

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init();
    
    std::cout<< "window with title: \"" << m_Title << "\" was created successfully \n";
    std::cout << glGetString(GL_VERSION) << " OpenGL Driver Version \n";

    int maxSamples;
    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
    int maxTexturesFrag;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTexturesFrag);
    int maxTexturesVert;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxTexturesVert);
    std::cout << "max samplers: " << maxSamples << ", max texture units in frag: " << maxTexturesFrag <<
         ",max texture units in vert: " << maxTexturesVert << "\n";
}

void window_gl::setTitle(std::string title) {
    glfwSetWindowTitle(m_Window, title.c_str());
    m_Title = title;
}

std::string window_gl::getTitle() {
    return m_Title;
}

int window_gl::getTargetFrameRate() {
    return m_TargetFrameRate;
}

void window_gl::setTargetFrameRate(int fps) {
    m_TargetFrameRate = std::clamp(fps,-1,std::numeric_limits<int>::max());
}

long double window_gl::getTimeSinceStart() {
    return m_elapsedTimeSinceStart;
}

void window_gl::setVsync(bool enabled) {
    glfwSwapInterval(enabled ? 1 : 0);
}

void window_gl::onScrollChanged(double yOffset) {

}

void window_gl::subscribeEvents() {
    
    static auto callback_static = [this](GLFWwindow* window, double xoffset, double yoffset){
        onScrollChanged(yoffset);
    };
    
    glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
                       {
                           if(ImGui::GetIO().WantCaptureMouse)
                           {
                               return;
                           }
                           callback_static(window, xOffset, yOffset);
                       }
    );

    static auto callback_static_2 = [this](GLFWwindow* window, int button, int action, int mods){
        onMouseButtonChanged(button, action, mods);
    };

    glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
                          {
                              if(ImGui::GetIO().WantCaptureMouse)
                              {
                                  return;
                              }
                              callback_static_2(window, button, action, mods);
                          }
    );

    static auto callback_static_3 = [this](GLFWwindow* window, double xpos, double ypos){
        onMousePositionChanged(xpos, ypos);
    };

    glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xpos, double ypos)
                               {
                                   if(ImGui::GetIO().WantCaptureMouse)
                                   {
                                       return;
                                   }
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

void window_gl::onMouseButtonChanged(int button, int action, int mods) {
    

}

void window_gl::onMousePositionChanged(double xpos, double ypos) {
    
}

void window_gl::onSizeChanged(int height, int width) {
    m_Height = height;
    m_Width = width;

    m_Paused = m_Height == 0 || m_Width == 0;
}

void window_gl::onRenderImGUI() {

}

