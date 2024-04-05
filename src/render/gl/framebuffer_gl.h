
#ifndef FRAMEBUFFER_GL_H
#define FRAMEBUFFER_GL_H

#include "../framebuffer.h"
#include "texture_2D_gl.h"
#include "texture_cube_gl.h"
#include "texture_2D_array_gl.h"

namespace modelViewer::render
{
	class framebuffer_gl : public framebuffer {
		
	public:
		framebuffer_gl(std::string& name);
		~framebuffer_gl() override;
		
		void unbind() override;

		void createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare) override;
		void createDepthTexture(int width, int height, bool enableDepthCompare) override;
		void createCubeMap(int size) override;
		void createColorBuffer(int width, int height) override;
		
		void attachCubeMapFace(int index) override;
		void attachDepthTextureArray(int layer) override;
		void attachDepthTexture() override;
		
		void clearColorBuffer(const glm::vec4& color) override;
		void clearDepthBuffer() override;
	
		std::shared_ptr<texture> getDepthMap() override;
		std::shared_ptr<texture> getDepthMapArray() override;
		std::shared_ptr<texture> getColorCubeMap() override;

	private:
		std::string  m_Name;
		unsigned int m_BufferId = -1;
		
		std::shared_ptr<texture_2D_gl> m_DepthTexture;
		std::shared_ptr<texture_cube_gl> m_ColorCubeTexture;
		std::shared_ptr<texture_2D_array_gl> m_DepthArrayTexture;
	};

}


#endif //FRAMEBUFFER_H
