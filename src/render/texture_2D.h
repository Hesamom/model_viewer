#ifndef MODEL_VIEWER_TEXTURE_2D_H
#define MODEL_VIEWER_TEXTURE_2D_H


#include "../resource/texture_asset.h"
#include "texture_setup.h"
#include "texture.h"

namespace modelViewer::render {
    class texture_2D : public texture{
    private:
        texture_setup m_Setup;
        
        bool isTextureResident();
        void setFilteringModeMin(texture_filtering_mode textureFiltering);
        void setFilteringModeMag(texture_filtering_mode textureFiltering);
    protected:
        unsigned int GetTextureType() override{return GL_TEXTURE_2D;};
        modelViewer::res::texture_asset_type GetTextureAssetType()const override{return m_Setup.type;};
    public:
        texture_2D(texture_setup& texture_setup);
        ~texture_2D();
        texture_filtering_mode  getFilteringModeMin() const;
        texture_filtering_mode  getFilteringModeMag() const;
        texture_wrapping_mode getWrappingMode() const;
        unsigned int getMipMapMinLevel();
        unsigned int getMipMapMaxLevel();
        void setMipMapLevels(unsigned int min, unsigned int max);
        void setFilteringMode(texture_filtering_mode textureFilteringMin, texture_filtering_mode textureFilteringMag);
        void setWrappingMode(texture_wrapping_mode textureWrapping);
    };
}

#endif
