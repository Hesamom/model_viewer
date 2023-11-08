#ifndef MODEL_VIEWER_WINDOW_H
#define MODEL_VIEWER_WINDOW_H

#include "GLFW/glfw3.h"

class window{
private:
    GLFWwindow* m_Window;

public:
    window(int width, int height, const char* windowTitle);
    ~window();
    bool isOpen();
};


#endif

