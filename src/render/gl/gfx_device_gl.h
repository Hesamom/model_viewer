
#ifndef GFX_DEVICE_GL_H
#define GFX_DEVICE_GL_H
#include "../gfx_device.h"
#include "../../window/window_glfw.h"

namespace modelViewer::render {
	class gfx_device_gl : public gfx_device {

	public:
		explicit gfx_device_gl(std::shared_ptr<window_glfw>& window);
		void swapBuffers() override;
		void setViewport(int width, int height) override;
		void clearDepthBuffer() override;
		void resize(int width, int height) override;
		void clearColorBuffer(const glm::vec4& color) override;
		void* getDevice() override;
		void popDebugGroup() override;
		void pushDebugGroup(const char* label) override;
		void onStartRender() override;
		
		void onPreRenderImGUI() override;
		void onPostRenderImGUI() override;
		void onInitImGUI() override;
		void onShutdownImGUI() override;
		
		int getMaxSamplersPerProgram() override;

		std::shared_ptr<texture> createTextureCube(texture_setup &setup) override ;
		std::shared_ptr<texture> createTexture2D(texture_setup &setup) override ;

		std::shared_ptr<mesh> createMesh(std::shared_ptr<res::mesh_asset>& asset) override;
		std::shared_ptr<shader_program> createProgram(std::vector<std::shared_ptr<res::shader_asset>>& assets) override;
		std::shared_ptr<framebuffer> createFramebuffer() override;
	
	private:

		std::shared_ptr<window_glfw> m_Window;

		void initDevice();
	};
}


#endif //GFX_DEVICE_GL_H
