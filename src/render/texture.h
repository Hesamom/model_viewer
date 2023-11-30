#ifndef MODEL_VIEWER_TEXTURE_H
#define MODEL_VIEWER_TEXTURE_H


#include "../resource/texture_asset.h"

namespace modelViewer::render {
    
    //TODO rename enums to match other class names 
    //TODO only specify per texture filtering mode (linear, nearest)
    enum class textureFiltering
    {
        nearest,
        linear,
        nearest_Mipmap_Nearest,
        linear_Mipmap_Nearest,
        nearest_Mipmap_Linear,
        linear_Mipmap_Linear
    };
    
    enum class texture_mipmap_mode
    {
        none,
        linear,
        nearest
    };
    enum class textureWrapping
    {
        clamp_To_Edge,
        mirror_Clamp_ToEdge,
        clamp_To_Border,
        repeat,
        mirrored_Repeat,
        wrap
    };
    
    
    class texture{
    private:
        std::shared_ptr<modelViewer::res::texture_asset> m_Asset;
        unsigned int m_TextureId;
        unsigned int m_MipMapMinLevel;
        unsigned int m_MipMapMaxLevel;
        bool m_IsMipMapActive;
        textureFiltering m_TextureFilteringMin = textureFiltering::linear;
        textureFiltering m_TextureFilteringMig = textureFiltering::linear;
        textureWrapping m_TextureWrapping = textureWrapping::clamp_To_Edge;

        void setFilteringModeMin(textureFiltering textureFiltering);
        void setFilteringModeMig(textureFiltering textureFiltering);
    public:
        //TODO wrap these params in a struct/class 
        explicit texture(std::shared_ptr<modelViewer::res::texture_asset> asset, textureFiltering m_TextureFilteringMin = textureFiltering::linear, textureFiltering textureFilteringMig = textureFiltering::linear, textureWrapping textureWrapping = textureWrapping::clamp_To_Edge, bool isMipMapActive = false, unsigned int mipMapMinLevel = 2, unsigned int mipMapMaxLevel = 16);
        ~texture();
        void bind() const;
        textureFiltering  getFilteringModeMin() const;
        textureFiltering  getFilteringModeMig() const;
        textureWrapping getWrappingMode() const;
        unsigned int getMipMapMinLevel();
        unsigned int getMipMapMaxLevel();
        void setMipMapMinLevel(unsigned int minLevel);
        void setMipMapMaxLevel(unsigned int maxLevel);
        void setFilteringMode(textureFiltering textureFilteringMin, textureFiltering textureFilteringMig);
        void setWrappingMode(textureWrapping textureWrapping);
    };
}

#endif
