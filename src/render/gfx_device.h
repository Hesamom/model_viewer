
#ifndef GFXDEVICE_H
#define GFXDEVICE_H

#include "glm/vec3.hpp"

class gfx_device
{
public:
	virtual void swapBuffers() = 0;
	virtual void setViewport(int width, int height) = 0;
	virtual void setClearColor(glm::vec3& color) = 0;
};

#endif //GFXDEVICE_H
