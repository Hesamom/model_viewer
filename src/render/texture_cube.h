#ifndef MODEL_VIEWER_TEXTURE_CUBE_H
#define MODEL_VIEWER_TEXTURE_CUBE_H

#include "GL/glew.h"
#include "texture.h"
#include "texture_setup.h"

namespace modelViewer::render {
    class texture_cube: public texture {
    public:
        explicit texture_cube(texture_setup& textureSetup);
        virtual unsigned int GetTextureType() override{return GL_TEXTURE_CUBE_MAP;};
        virtual res::texture_asset_type GetTextureAssetType() const override{return res::texture_asset_type::textureCube;};
    };
}


#endif
