﻿#ifndef TEXTURE_CUBE_GL_H
#define TEXTURE_CUBE_GL_H

#include "GL/glew.h"
#include "texture_gl.h"
#include "../texture_setup.h"

namespace modelViewer::render {
    class texture_cube: public texture_gl {
    public:
        explicit texture_cube(texture_setup& textureSetup);
		render::shader_texture_type getType() const override;
        unsigned int GetTextureType() override{return GL_TEXTURE_CUBE_MAP;};
        res::texture_asset_type GetTextureAssetType() const override{return res::texture_asset_type::textureCube;};
    };
}


#endif
