﻿#ifndef MODEL_VIEWER_WINDOW_H
#define MODEL_VIEWER_WINDOW_H

#include "GLFW/glfw3.h"

class window{
private:
    GLFWwindow* m_Window = nullptr;
    GLFWmonitor* m_Monitor = nullptr;
    std::string  m_Title;
    int m_Width = 0; 
    int m_Height = 0;
    
    void initContext();
    
protected:
    virtual void onRender();
    virtual void onInit();
    virtual void onSizeChanged(int width, int height);
    
public:
    window(int width, int height, const std::string& title, bool fullscreen);
    virtual ~window();
    
    void setSize(int width, int height);
    int getHeight();
    int getWidth();
    void setTitle(std::string title);
    std::string  getTitle();
    void draw();
    bool isOpen();
};

#endif

