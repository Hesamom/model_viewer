#ifndef MODEL_VIEWER_RENDER_SCENE_H
#define MODEL_VIEWER_RENDER_SCENE_H

#include "object_renderer.h"
#include "light_point.h"
#include "light_spot.h"

namespace modelViewer::render {
    class render_scene {
    private:
        std::vector<std::shared_ptr<object_renderer>> m_StaticObjects;
    	std::vector<std::shared_ptr<object_renderer>> m_AllObjects;
		std::vector<std::shared_ptr<object_renderer>> m_ModelObjects;
        light_directional m_Light;
		const int MaxModelObjects = 1;
		std::vector<light_point> m_PointLights;
    	std::vector<light_spot> m_SpotLights;
    public:
        explicit render_scene(std::vector<std::shared_ptr<object_renderer>> renderObjects);
        render_scene();
        
        //TODO: make it implementation independent 
        std::vector<std::shared_ptr<object_renderer>>& getObjects();
        void addStaticObject(std::shared_ptr<object_renderer>& object);
        void removeStaticObject(std::shared_ptr<object_renderer>& object);

        void addModelObject(std::shared_ptr<object_renderer>& object);
		void addPointLight(light_point& light);

        void removePointLight(int index);

        void addSpotLight(light_spot& light);

        void removeSpotLight(int index);

        std::vector<light_point>& getPointLights();
        std::vector<light_spot>& getSpotLights();
        light_directional& getDirectionalLight();
    };
}
#endif
