
#ifndef GFXDEVICE_H
#define GFXDEVICE_H

#include "framebuffer.h"
#include "shader_program.h"
#include "texture.h"
#include "texture_setup.h"
#include "mesh.h"

namespace modelViewer::render {

	enum class cull_face_mode {
		front,
		back
	};
	
	class gfx_device
	{
	public:
		virtual ~gfx_device() = default;

		virtual void swapBuffers() = 0;
		virtual void resize(int width, int height) = 0;
		virtual void setViewport(int width, int height) = 0;
		virtual void setClearColor(glm::vec4& color) = 0;
		virtual void setCullFaceMode(cull_face_mode mode) = 0;
		virtual int getMaxSamplersPerProgram() = 0;
		virtual void setDepthmap(bool enable) = 0;
		virtual void setCullFace(bool enable) = 0;
		virtual void clearColorBuffer(const glm::vec4& color) = 0;
		virtual void clearDepthBuffer() = 0;
		virtual void* getDevice() = 0;
		
		virtual void onRenderImGUI() = 0;
		virtual void onPostRenderImGUI() = 0;
		virtual void onInitImGUI() =0;
		virtual void onShutdownImGUI() = 0;
		
		virtual void pushDebugGroup(const char* label) = 0;
		virtual void popDebugGroup() = 0;
		
		virtual std::shared_ptr<texture> createTexture2D(texture_setup& setup) = 0;
		virtual std::shared_ptr<texture> createTextureCube(texture_setup& setup) = 0;
		virtual std::shared_ptr<mesh> createMesh(std::shared_ptr<res::mesh_asset>& asset) = 0;
		virtual std::shared_ptr<shader_program> createProgram(std::vector<std::shared_ptr<res::shader_asset>>& assets) = 0;
		virtual std::shared_ptr<framebuffer> createFramebuffer() = 0;
	};


}

#endif //GFXDEVICE_H
