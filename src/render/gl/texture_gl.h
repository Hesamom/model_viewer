#ifndef MODEL_VIEWER_TEXTURE_H
#define MODEL_VIEWER_TEXTURE_H

#include "../../resource/model_info.h"
#include "../texture.h"
#include "glew/include/GL/glew.h"

namespace modelViewer::render {
    
    class texture_gl : public texture {
    protected:
        ~texture_gl() = default;

        unsigned int m_TextureId = -1;

        void setBind(bool bind) {
            
            glBindTexture(GetTextureType(), m_TextureId);
        }

        virtual unsigned int GetTextureType() = 0;
        virtual res::texture_asset_type GetTextureAssetType() const = 0;
 
    public:
        virtual void active(const int index) override
        {
            constexpr int firstIndex = GL_TEXTURE0;
            const int slotIndex = firstIndex + index;
            glActiveTexture(slotIndex);
            setBind(true);
        }

        res::texture_asset_type getType() const {
            return GetTextureAssetType();
        }
    };
}


#endif
