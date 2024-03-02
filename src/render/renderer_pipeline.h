
#ifndef RENDERER_PIPELINE_H
#define RENDERER_PIPELINE_H

namespace modelViewer::render {
    
    enum class clear_mode
    {
        color ,
        skybox
    };

    
    class renderer_pipeline {
    public:
        virtual void render(render_scene& scene, camera& camera, bool shadowEnabled, bool reflectionEnabled) = 0;
        virtual void setClearFlag(glm::vec4 color) = 0;
        virtual void setReflectionPosition(const glm::vec3& pos) = 0;
        virtual glm::vec3& getReflectionPosition() = 0;
        virtual void setClearMode(clear_mode mode) = 0;
        virtual void setReflectionClearMode(clear_mode mode) = 0;
        virtual void setReflectionClearFlag(const glm::vec4& color) = 0;

        virtual ~renderer_pipeline() = default;
    };
}

#endif //RENDERER_PIPELINE_H
