
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace modelViewer::render
{
	class framebuffer {
	private:
		unsigned int m_BufferId = -1;
		unsigned int m_depthRenderBuffer = -1;
		unsigned int m_CubeMapId = -1;
		unsigned int m_DepthTextureId = -1;
		unsigned int m_ArrayDepthTextureId = -1;
		
	public:
		framebuffer();
		~framebuffer();
		void bind();

		unsigned int createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare);
		unsigned int createDepthTexture(int width, int height, bool enableDepthCompare, std::string& name);
		unsigned int createCubeMap(int size, std::string& name);
		void attachCubeMapFace(int index);
		void attachDepthTexture();
		void attachDepthTextureArray(int layer);
		
		void unbind();
		void activateDepthMap(int slot);
		void activateDepthMapArray(int slot);
		void activateCubeMap(int slot) const;
	};

}


#endif //FRAMEBUFFER_H
