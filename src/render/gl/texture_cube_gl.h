#ifndef TEXTURE_CUBE_GL_H
#define TEXTURE_CUBE_GL_H

#include "GL/glew.h"
#include "texture_gl.h"
#include "../texture_setup.h"

namespace modelViewer::render {
    class texture_cube_gl: public texture_gl {
    public:
        explicit texture_cube_gl(texture_setup& textureSetup);
		texture_cube_gl(int size, std::string& name);
		
		render::shader_texture_type getType() const override;
		res::texture_asset_type GetTextureAssetType() const override
		{
			return res::texture_asset_type::textureCube;
		};

		void BindAsFrameBuffer(int faceIndex);
		
	protected:
        unsigned int GetTextureType() override{return GL_TEXTURE_CUBE_MAP;};
    };
}


#endif
