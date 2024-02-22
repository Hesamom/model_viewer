
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace modelViewer::render
{
	class framebuffer {
		
	private:
		unsigned int m_Id = -1;
		unsigned int m_DepthTextureId = -1;
		unsigned int m_ColorTextureId = -1;
		unsigned int m_ArrayDepthTextureId = -1;
		unsigned int m_ArrayColorTextureId = -1;
	public:
		framebuffer();
		~framebuffer();
		void bind();

		unsigned int createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare);
		unsigned int createDepthTexture(int width, int height, bool enableDepthCompare);
		void attachDepthTexture();
		void attachDepthTextureArray(int layer);
		
		void unbind();
		void activateDepthMap(int slot);
		void activateDepthMapArray(int slot);

		unsigned int createColorTexture(int width, int height);

		void attachColorTexture();

		void attachColorTextureArray(int layer);

		unsigned int createArrayColorTexture(int width, int height, int layers);
	};

}


#endif //FRAMEBUFFER_H
