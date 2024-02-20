#ifndef MODEL_VIEWER_WINDOW_H
#define MODEL_VIEWER_WINDOW_H

#include "renderdoc.h"

class GLFWwindow;
class GLFWmonitor;

class window{
private:
    GLFWwindow* m_Window = nullptr;
    GLFWmonitor* m_Monitor = nullptr;
    std::string  m_Title;
    int m_Width = 0; 
    int m_Height = 0;
    int m_TargetFrameRate = -1;
    long double m_elapsedTimeSinceStart = 0;
    bool m_Paused = false;
	bool m_RequestedCapture = false;
	
	renderdoc m_RenderDoc;
	
    void initContext(bool vSync);
    void subscribeEvents();
    
protected:
    virtual void onRender(float elapsedTime);
    virtual void onScrollChanged(double yOffset);
    virtual void onMouseButtonChanged(int button, int action, int mods);
    virtual void onMousePositionChanged(double xpos, double ypos);
	virtual void onKeyboardCallback(int key, int scancode, int action, int mods);
    virtual void onSizeChanged(int height, int width);
    virtual void onRenderImGUI();
	virtual void beforeInitContext();
	void requestCapture();
    
public:
    window(int width, int height, const std::string& title, bool fullscreen, bool vSync = true, int mssaLevel = 8);
    virtual ~window();
    
    void setSize(int width, int height);
    int getHeight();
    int getWidth();
    int getTargetFrameRate();
    void setTargetFrameRate(int fps);
    void setTitle(std::string title);
    void setVsync(bool enabled);
    std::string  getTitle();
    void draw();
    bool isOpen();
    long double getTimeSinceStart();

	
};

#endif

