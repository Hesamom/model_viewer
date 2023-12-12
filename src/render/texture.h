#ifndef MODEL_VIEWER_TEXTURE_H
#define MODEL_VIEWER_TEXTURE_H

#include "../resource/model_info.h"
#include "gl/glew.h"

namespace modelViewer::render {
    class texture {
    protected:
        unsigned int m_TextureId = -1;

        void setBind(bool bind) {

            static unsigned int m_BoundTexture = -1;
            if(bind && m_BoundTexture == m_TextureId)
            {
                return;
            }

            if (bind)
            {
                m_BoundTexture = m_TextureId;
            }
            else
            {
                m_BoundTexture = 0;
            }

            glBindTexture(GetTextureType(), m_BoundTexture);
        }

        virtual unsigned int GetTextureType() = 0;
        virtual res::texture_asset_type GetTextureAssetType() const = 0;
    private:
    public:
        virtual void active(int index) {
            int firstIndex = GL_TEXTURE0;
            int slotIndex = firstIndex + index;
            glActiveTexture(slotIndex);
            setBind(true);
        }

        modelViewer::res::texture_asset_type getType() const {
            return GetTextureAssetType();
        }
    };
}


#endif
