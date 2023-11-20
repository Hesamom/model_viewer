#include "renderScene.h"

#include <utility>

using namespace modelViewer::render;

renderScene::renderScene(std::vector <renderObject> renderObjects) {
    m_RenderObjects = std::move(renderObjects);
}

std::vector<renderObject> renderScene::GetObjects() {
    return m_RenderObjects;
}
