
#ifndef GFX_DEVICE_GL_H
#define GFX_DEVICE_GL_H
#include "gfx_device.h"
#include "../window/window_glfw.h"


class gfx_device_gl : public gfx_device {

public:
	explicit gfx_device_gl(std::shared_ptr<window_glfw>& window);
	void swapBuffers() override;
	void setViewport(int width, int height) override;
	void setClearColor(glm::vec3& color) override;
	
private:

	std::shared_ptr<window_glfw> m_Window;

	void initDevice();
};


#endif //GFX_DEVICE_GL_H
