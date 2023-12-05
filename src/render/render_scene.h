#ifndef MODEL_VIEWER_RENDER_SCENE_H
#define MODEL_VIEWER_RENDER_SCENE_H

#include "render_object.h"

namespace modelViewer::render {
    class render_scene {
    private:
        std::vector<std::shared_ptr<render_object>> m_RenderObjects;
        light_directional m_Light;
    public:
        explicit render_scene(std::vector<std::shared_ptr<render_object>> renderObjects);
        render_scene();
        
        //TODO: make it implementation independent 
        std::vector<std::shared_ptr<render_object>>& getObjects();
        void addObject(std::shared_ptr<render_object>& object);
        light_directional& getLight();
    };
}
#endif
