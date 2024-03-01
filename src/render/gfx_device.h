
#ifndef GFXDEVICE_H
#define GFXDEVICE_H

#include "mesh.h"
#include "glm/vec3.hpp"
#include "texture.h"
#include "texture_setup.h"

class gfx_device
{
public:
	virtual ~gfx_device() = default;

	virtual void swapBuffers() = 0;
	virtual void setViewport(int width, int height) = 0;
	virtual void setClearColor(glm::vec3& color) = 0;

	virtual std::shared_ptr<texture> createTexture2D(modelViewer::render::texture_setup& setup) = 0;
	virtual std::shared_ptr<texture> createTextureCube(modelViewer::render::texture_setup& setup) = 0;

	virtual std::shared_ptr<modelViewer::render::mesh> createMesh(std::shared_ptr<modelViewer::res::mesh_asset>& asset) = 0;
};

#endif //GFXDEVICE_H
