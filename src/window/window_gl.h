#ifndef MODEL_VIEWER_WINDOW_H
#define MODEL_VIEWER_WINDOW_H

#include "window.h"

class GLFWwindow;
class GLFWmonitor;

class window_gl : public window {
private:
    GLFWwindow* m_Window = nullptr;
    GLFWmonitor* m_Monitor = nullptr;
    std::string  m_Title;
    int m_Width = 0; 
    int m_Height = 0;
    int m_TargetFrameRate = -1;
    long double m_elapsedTimeSinceStart = 0;
    bool m_Paused = false;

    void initContext(bool vSync);
    void subscribeEvents();
    
protected:
    virtual void onRender(float elapsedTime);
    virtual void onScrollChanged(double yOffset);
    virtual void onMouseButtonChanged(int button, int action, int mods);
    virtual void onMousePositionChanged(double xpos, double ypos);
    virtual void onSizeChanged(int height, int width);
    virtual void onRenderImGUI();
    
public:
    window_gl(int width, int height, const std::string& title, bool fullscreen, bool vSync = true, int mssaLevel = 8);
    virtual ~window_gl();
    
	void setSize(int width, int height) override;
    int getHeight() override;
    int getWidth() override;
    int getTargetFrameRate() override;
    void setTargetFrameRate(int fps) override;
    void setTitle(std::string title) override;
    void setVsync(bool enabled) override;
    std::string  getTitle() override;
    void draw() override;
    bool isOpen();
    long double getTimeSinceStart();

    
};

#endif

