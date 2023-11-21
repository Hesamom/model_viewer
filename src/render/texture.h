#ifndef MODEL_VIEWER_TEXTURE_H
#define MODEL_VIEWER_TEXTURE_H


#include "../resource/texture_asset.h"

namespace modelViewer::render {
    
    enum class textureFiltering
    {
        nearestNeighbors,
        linear,
        bilinear
    };
    enum class textureWrapping
    {
        clampToEdge,
        clampToBorder,
        wrap
    };
    
    
    class texture{
    private:
        std::shared_ptr<modelViewer::res::texture_asset> m_Asset;
        unsigned int m_TextureId;
    public:
        
        //TODO pass wrapping and filtering 
        explicit texture(std::shared_ptr<modelViewer::res::texture_asset> asset);
        ~texture();
        void bindTexture();
    };
}

#endif
