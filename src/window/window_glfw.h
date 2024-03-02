#ifndef MODEL_VIEWER_WINDOW_H
#define MODEL_VIEWER_WINDOW_H

#include "window.h"

class GLFWwindow;
class GLFWmonitor;

class window_glfw : public window {
private:
    GLFWwindow* m_Window = nullptr;
    GLFWmonitor* m_Monitor = nullptr;
    std::string  m_Title;
    int m_Width = 0; 
    int m_Height = 0;
    bool m_Paused = false;

    void subscribeEvents();

	std::function<void(int, int)>  m_SizeCallback;
    
protected:
    virtual void onScrollChanged(double yOffset);
    virtual void onMouseButtonChanged(int button, int action, int mods);
    virtual void onMousePositionChanged(double xpos, double ypos);
    virtual void onSizeChanged(int height, int width);
    
public:
    window_glfw(int width, int height, const std::string& title, bool fullscreen, bool vSync = true, int mssaLevel = 8);
    ~window_glfw() override;
	
	
	void setGlContext(int major, int minor, int mssa);
	void swapBuffers();

	void setOnSizeChangedCallback(std::function<void(int, int)> callback) override;
	void setSize(int width, int height) override;
    int getHeight() override;
    int getWidth() override;
    void setTitle(std::string title) override;
    std::string  getTitle() override;
    bool isOpen();

	bool shouldClose() override;
	bool isPaused() override;
	void pollEvents() override;
    
};

#endif

