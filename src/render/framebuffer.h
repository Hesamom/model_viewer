
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

namespace modelViewer::render
{
	class framebuffer {
		
	private:
		unsigned int m_Id = -1;
		unsigned int m_DepthTextureId = -1;
	public:
		framebuffer();
		~framebuffer();
		void bind();
		void unbind();
		void attachDepth(int width, int height, bool enableDepthCompare);
		void activateDepthMap(int slot);
	};

}


#endif //FRAMEBUFFER_H
