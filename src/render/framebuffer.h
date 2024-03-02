#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
#include <glm/vec4.hpp>

namespace modelViewer::render {
    class framebuffer {
    public:

        virtual ~framebuffer() = default;
        virtual void bind() = 0;
        virtual void unbind() = 0;
        
        virtual void createArrayDepthTexture(int width, int height, int layers, bool enableDepthCompare) = 0;
        virtual void createDepthTexture(int width, int height, bool enableDepthCompare, std::string& name) = 0;
        virtual void createCubeMap(int size, std::string& name) = 0;
        virtual void clearColorBuffer(const glm::vec4& color) = 0;
        virtual void clearDepthBuffer() = 0;
        
        virtual void attachCubeMapFace(int index) = 0;
        virtual void attachDepthTexture() = 0;
        virtual void attachDepthTextureArray(int layer) = 0;
        virtual void activateDepthMap(int slot) = 0;
        virtual void activateDepthMapArray(int slot) = 0;
        virtual void activateCubeMap(int slot) const  = 0;
    };
}

#endif //FRAMEBUFFER_H
