
#ifndef RENDERER_SIMPLE_H
#define RENDERER_SIMPLE_H
#include "camera.h"
#include "renderer_pipeline.h"
#include "render_scene.h"


namespace modelViewer::render
{
    class renderer_simple : public renderer_pipeline {
        
    public:
        renderer_simple(std::shared_ptr<gfx_device>& device);
        void render(render_scene& scene, camera& camera, bool shadowEnabled, bool reflectionEnabled);
        void setClearFlag(glm::vec4 color);
        void setReflectionPosition(const glm::vec3& pos);
        glm::vec3& getReflectionPosition();
        void setClearMode(clear_mode mode);
        void setReflectionClearMode(clear_mode mode);
        void setReflectionClearFlag(const glm::vec4& color);

    private:
        glm::vec3 m_ReflectPos;
        std::shared_ptr<gfx_device> m_Device;
        glm::vec4 m_Color;
    };
}



#endif //RENDERER_SIMPLE_H
