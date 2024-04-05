#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <glm/vec4.hpp>
#include "texture.h"

namespace modelViewer::render {
    class framebuffer {
    public:

        virtual ~framebuffer() = default;

        virtual void unbind() = 0;
        
        virtual void createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare) = 0;
        virtual void createDepthTexture(int width, int height, bool enableDepthCompare) = 0;
        virtual void createCubeMap(int size) = 0;
		virtual void createColorBuffer(int width, int height) = 0;
		
        virtual void clearColorBuffer(const glm::vec4& color) = 0;
        virtual void clearDepthBuffer() = 0;
        
        virtual void attachCubeMapFace(int index) = 0;
        virtual void attachDepthTextureArray(int layer) = 0;
		virtual void attachDepthTexture() = 0;
		
        virtual std::shared_ptr<texture> getDepthMap() = 0;
		virtual std::shared_ptr<texture> getDepthMapArray() = 0;
		virtual std::shared_ptr<texture> getColorCubeMap()  = 0;
    };
}

#endif //FRAMEBUFFER_H
