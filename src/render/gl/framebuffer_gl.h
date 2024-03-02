
#ifndef FRAMEBUFFER_GL_H
#define FRAMEBUFFER_GL_H

#include "../framebuffer.h"

namespace modelViewer::render
{
	class framebuffer_gl : public framebuffer {
		
	public:
		framebuffer_gl();
		~framebuffer_gl() override;
		
		void bind() override;
		void unbind() override;

		void createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare) override;
		void createDepthTexture(int width, int height, bool enableDepthCompare, std::string& name) override;
		void createCubeMap(int size, std::string& name) override;
		void attachCubeMapFace(int index) override;
		void attachDepthTexture() override;
		void attachDepthTextureArray(int layer) override;
		void clearColorBuffer(const glm::vec4& color) override;
		void clearDepthBuffer() override;
	
		void activateDepthMap(int slot) override;
		void activateDepthMapArray(int slot) override;
		void activateCubeMap(int slot) const override;

	private:
		unsigned int m_BufferId = -1;
		unsigned int m_depthRenderBuffer = -1;
		unsigned int m_CubeMapId = -1;
		unsigned int m_DepthTextureId = -1;
		unsigned int m_ArrayDepthTextureId = -1;
	};

}


#endif //FRAMEBUFFER_H
