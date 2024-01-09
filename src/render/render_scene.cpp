#include "render_scene.h"

#include <utility>

#include "light_spot.h"


using namespace modelViewer::render;

render_scene::render_scene(std::vector<std::shared_ptr<render_object>> renderObjects) : m_StaticObjects{std::move(renderObjects)}
{
    
}

std::vector<std::shared_ptr<render_object>>& render_scene::getObjects() {

	//TODO ineffient, optimize it later 
	m_AllObjects.clear();
	m_AllObjects.insert(m_AllObjects.end(), m_StaticObjects.begin(), m_StaticObjects.end());
	m_AllObjects.insert(m_AllObjects.end(), m_ModelObjects.begin(), m_ModelObjects.end());

	return m_AllObjects;
}

render_scene::render_scene() {

}

void render_scene::addStaticObject(std::shared_ptr<render_object> &object) {
    m_StaticObjects.push_back(object);
}

void render_scene::removeStaticObject(std::shared_ptr<render_object> &object) {
	const auto it = std::find(m_StaticObjects.begin(), m_StaticObjects.end(), object);
	if (it == m_StaticObjects.end()) {
		return;
	}
	
	m_StaticObjects.erase(it);
}

light_directional &render_scene::getDirectionalLight() {
    return m_Light;
}

void render_scene::addModelObject(std::shared_ptr<render_object>& object)
{
	if(m_ModelObjects.size() + 1 > MaxModelObjects)
	{
		m_ModelObjects.erase(m_ModelObjects.end() - 1, m_ModelObjects.end());
	}
	
	m_ModelObjects.push_back(object);
}

void render_scene::addPointLight(light_point& light)
{
	m_PointLights.push_back(light);
}

void render_scene::removePointLight(int index) {
	if (index < 0 || index >= m_PointLights.size()) {
		throw std::range_error("index is not in range");
	}

	const auto iterator = m_PointLights.begin() + index;
	m_PointLights.erase(iterator);
}

void render_scene::addSpotLight(light_spot& light) {
	m_SpotLights.push_back(light);
}

void render_scene::removeSpotLight(int index) {
	if (index < 0 || index >= m_SpotLights.size()) {
		throw std::range_error("index is not in range");
	}

	const auto iterator = m_SpotLights.begin() + index;
	m_SpotLights.erase(iterator);
}


std::vector<light_point>& render_scene::getPointLights()
{
	return m_PointLights;
}

std::vector<light_spot>& render_scene::getSpotLights() {
	return m_SpotLights;
}
	

