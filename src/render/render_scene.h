#ifndef MODEL_VIEWER_RENDER_SCENE_H
#define MODEL_VIEWER_RENDER_SCENE_H

#include "render_object.h"
#include "light_point.h"

namespace modelViewer::render {
    class render_scene {
    private:
        std::vector<std::shared_ptr<render_object>> m_Objects;
		std::vector<std::shared_ptr<render_object>> m_ModelObjects;
        light_directional m_Light;
		const int MaxModelObjects = 1;
		std::vector<light_point> m_PointLights;
		
    public:
        explicit render_scene(std::vector<std::shared_ptr<render_object>> renderObjects);
        render_scene();
        
        //TODO: make it implementation independent 
        std::vector<std::shared_ptr<render_object>>& getObjects();
        void addStaticObject(std::shared_ptr<render_object>& object);
		void addModelObject(std::shared_ptr<render_object>& object);
		void addPointLight(light_point& light);
		std::vector<light_point>& getPointLights();
        light_directional& getLight();
    };
}
#endif
