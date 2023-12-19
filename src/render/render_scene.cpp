#include "render_scene.h"

#include <utility>

#include "light_spot.h"


using namespace modelViewer::render;

render_scene::render_scene(std::vector<std::shared_ptr<render_object>> renderObjects) : m_Objects{std::move(renderObjects)}
{
    
}

std::vector<std::shared_ptr<render_object>>& render_scene::getObjects() {
    return m_Objects;
}

render_scene::render_scene() {

}

void render_scene::addStaticObject(std::shared_ptr<render_object> &object) {
    m_Objects.push_back(object);
}

light_directional &render_scene::getLight() {
    return m_Light;
}

void render_scene::addModelObject(std::shared_ptr<render_object>& object)
{
	if(m_ModelObjects.size() + 1 > MaxModelObjects)
	{
		m_Objects.erase(m_Objects.end() - 1, m_Objects.end());
		m_ModelObjects.erase(m_ModelObjects.end() - 1, m_ModelObjects.end());
	}
	
	m_ModelObjects.push_back(object);
	m_Objects.push_back(object);
}

void render_scene::addPointLight(light_point& light)
{
	m_PointLights.push_back(light);
}

void render_scene::addSpotLight(light_spot& light) {
	m_SpotLights.push_back(light);
}

std::vector<light_point>& render_scene::getPointLights()
{
	return m_PointLights;
}

std::vector<light_spot>& render_scene::getSpotLights() {
	return m_SpotLights;
}
	

