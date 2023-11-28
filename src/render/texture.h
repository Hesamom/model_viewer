#ifndef MODEL_VIEWER_TEXTURE_H
#define MODEL_VIEWER_TEXTURE_H


#include "../resource/texture_asset.h"

namespace modelViewer::render {
    
    enum class textureFiltering
    {
        nearest,
        linear,
        nearest_Mipmap_Nearest,
        linear_Mipmap_Nearest,
        nearest_Mipmap_Linear,
        linear_Mipmap_Linear
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
        textureFiltering m_TextureFiltering = textureFiltering::linear;
        textureWrapping m_TextureWrapping = textureWrapping::clamp_To_Edge;
    public:
        
        //TODO pass wrapping and filtering 
        explicit texture(std::shared_ptr<modelViewer::res::texture_asset> asset, textureFiltering textureFiltering = textureFiltering::linear ,textureWrapping textureWrapping = textureWrapping::clamp_To_Edge);
        ~texture();
        void bind() const;
        textureFiltering  getFilteringMode() const;
        textureWrapping getWrappingMode() const;
        void setFilteringMode(textureFiltering textureFiltering);
        void setWrappingMode(textureWrapping textureWrapping);
    };
}

#endif
