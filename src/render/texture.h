#ifndef MODEL_VIEWER_TEXTURE_H
#define MODEL_VIEWER_TEXTURE_H


#include "../resource/texture_asset.h"
#include "texture_setup.h"

namespace modelViewer::render {
    class texture{
    private:
        unsigned int m_TextureId;
        texture_setup m_Setup;
        
        bool isTextureResident();
        void setFilteringModeMin(texture_filtering_mode textureFiltering);
        void setFilteringModeMag(texture_filtering_mode textureFiltering);
        void setBind(bool bind);
    public:
        texture(texture_setup& texture_setup);
        ~texture();
        void active(int index);
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
