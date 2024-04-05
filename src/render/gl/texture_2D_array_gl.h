
#ifndef TEXTURE_2D_ARRAY_GL_H
#define TEXTURE_2D_ARRAY_GL_H

#include "texture_gl.h"

namespace modelViewer::render
{
	struct texture_2D_array_gl_desc
	{
		int width = 0;
		int height = 0;
		int layers = 0;
		bool enableDepthCompare = false;
		int format = 0;
		int type = 0;
		std::string name;
	};
	
	class texture_2D_array_gl : public texture_gl {
	protected:
		unsigned int GetTextureType() override{return GL_TEXTURE_2D_ARRAY;};
		modelViewer::res::texture_asset_type GetTextureAssetType() const override{ return 
		modelViewer::res::texture_asset_type::texture2DArray;};
	public:
		render::shader_texture_type getType() const override;
		explicit texture_2D_array_gl(texture_2D_array_gl_desc& desc);
		void BindAsFrameBuffer(int layer);
	};

}
#endif //TEXTURE_2D_ARRAY_GL_H
