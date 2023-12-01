#ifndef MODEL_VIEWER_TEXTURE_H
#define MODEL_VIEWER_TEXTURE_H


#include "../resource/texture_asset.h"
#include "texture_setup.h"
#include "GL/glew.h"

namespace modelViewer::render {
    class texture{
    private:
        std::shared_ptr<modelViewer::res::texture_asset> m_Asset;
        unsigned int m_TextureId;
        unsigned int m_MipMapMinLevel;
        unsigned int m_MipMapMaxLevel;
        bool m_IsMipMapActive;
        texture_filtering_mode m_TextureFilteringMin = texture_filtering_mode::linear;
        texture_filtering_mode m_TextureFilteringMig = texture_filtering_mode::linear;
        texture_wrapping_mode m_TextureWrapping = texture_wrapping_mode::clamp_to_edge;

        bool isTextureOutOfMemory();
        void setFilteringModeMin(texture_filtering_mode textureFiltering);
        void setFilteringModeMag(texture_filtering_mode textureFiltering);
        void setFilteringMode(texture_filtering_mode textureFiltering, GLint filterType);
    public:
        texture(texture_setup texture_setup);
        ~texture();
        void bind() const;
        texture_filtering_mode  getFilteringModeMin() const;
        texture_filtering_mode  getFilteringModeMag() const;
        texture_wrapping_mode getWrappingMode() const;
        unsigned int getMipMapMinLevel();
        unsigned int getMipMapMaxLevel();
        void setMipMapMinLevel(unsigned int minLevel);
        void setMipMapMaxLevel(unsigned int maxLevel);
        void setFilteringMode(texture_filtering_mode textureFilteringMin, texture_filtering_mode textureFilteringMig);
        void setWrappingMode(texture_wrapping_mode textureWrapping);
    };
}

#endif
