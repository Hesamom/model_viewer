#include "render_scene.h"

#include <utility>

using namespace modelViewer::render;

render_scene::render_scene(std::vector<std::shared_ptr<render_object>> renderObjects) : m_RenderObjects{renderObjects}
{
    
}

std::vector<std::shared_ptr<render_object>>& render_scene::getObjects() {
    return m_RenderObjects;
}

render_scene::render_scene() {

}

void render_scene::addObject(std::shared_ptr<render_object> &object) {
    m_RenderObjects.push_back(object);
}

