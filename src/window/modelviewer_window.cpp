
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "modelviewer_window.h"
#include "glfw/glfw3.h"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;

void modelviewer_window::onRender(float elapsed) {
    glClearBufferfv(GL_COLOR, 0, &m_ClearFlag.x);
    
    for (auto& info: m_NewModelsQueue) {
        auto program = getProgram(info);
        auto mesh = getMesh(info);
        auto texture = getTexture(info);
        auto object = std::make_shared<render_object>(program, mesh, texture, info.name);
        object->setTransform(info.transform);
        m_Scene.addObject(object);
    }

    m_NewModelsQueue.clear();


    auto viewMatrix = glm::lookAt(
            m_CameraPosition, // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0));  // Head is up (set to 0,-1,0 to look upside-down
            
    auto aspectRatio = (float)getWidth()/getHeight();
    auto projection = glm::perspective<float>(glm::radians(45.0),aspectRatio,0.1,100.0);
    
    auto viewProjection = projection * viewMatrix;
    for (auto& object : m_Scene.getObjects()) {
        
        object->render(viewProjection);
    }
}


void modelviewer_window::addModel(modelViewer::res::model_info& info) {
    
    m_NewModelsQueue.push_back(info);
}

std::shared_ptr<modelViewer::render::shader_program> modelviewer_window::getProgram(model_info &info) {

    auto vertShaderAsset = m_ShaderLoader.load(info.vertexShaderPath, shaderType::vertex );
    auto fragShaderAsset = m_ShaderLoader.load(info.fragmentShaderPath, shaderType::fragment);
    
    auto vertShader = std::make_shared<shader>(vertShaderAsset);
    vertShader->compile();
    if(!vertShader->isCompiled())
    {
        std::cerr<< vertShader->getCompilationLog() << std::endl;
    }
    auto fragShader = std::make_shared<shader>(fragShaderAsset);
    fragShader->compile();
    if(!fragShader->isCompiled())
    {
        std::cerr<< fragShader->getCompilationLog() << std::endl;
    }
    
    auto program = std::make_shared<shader_program>(std::initializer_list<std::shared_ptr<shader>>{vertShader, 
                                                                                                fragShader});
    if(!program->isLinked())
    {
        std::cerr<< program->getLinkLog() << std::endl;
    }

    return program;
}

std::shared_ptr<modelViewer::render::mesh> modelviewer_window::getMesh(model_info &info) {
    
    auto meshAsset = m_MeshLoader.load(info.meshPath);
    auto meshPtr  = std::make_shared<mesh>(meshAsset);
    
    return meshPtr;
}

std::shared_ptr<modelViewer::render::texture> modelviewer_window::getTexture(model_info &info) {

    auto textureAsset = m_TextureLoader.load(info.texturePath);
    auto texturePtr = std::make_shared<texture>(textureAsset);
    
    return texturePtr;
}

modelviewer_window::modelviewer_window(int width, int height, std::string title, bool fullscreen) : window(width,
                                                                                                           height,
                                                                                                           title,
                                                                                                           fullscreen) {
    
    setVsync(false);
    setTargetFrameRate(360);
    updateCameraPosition();
}

modelviewer_window::~modelviewer_window() {
    
}

void modelviewer_window::setClearFlag(glm::vec4 color) {
    m_ClearFlag = color;
}


glm::vec3 getPosition(float pitch, float yaw, float zoomLevel)
{
    float xPos = cos(glm::radians(pitch)) * sin(glm::radians(yaw)) * zoomLevel;
    float yPos = sin(glm::radians(pitch)) * zoomLevel;
    float zPos = cos(glm::radians(pitch)) * cos(glm::radians(yaw)) * zoomLevel;
    return {xPos,yPos, zPos};
}

void modelviewer_window::onScrollChanged(double yOffset) {
    
    //down -1, up 1
    m_ZoomLevel += yOffset * -1 * 3;
    m_ZoomLevel = std::clamp<float>(m_ZoomLevel, 1, 50);
    
    updateCameraPosition();
}

void modelviewer_window::onMouseButtonChanged(int button, int action, int mods) {
    
    if (button !=  GLFW_MOUSE_BUTTON_MIDDLE)
    {
        return;
    }

    m_IsMouseButtonDown = action == GLFW_REPEAT || action == GLFW_PRESS;
}


void modelviewer_window::onMousePositionChanged(double xpos, double ypos) {
    
    if (m_IsMouseButtonDown)
    {
        auto deltaY = m_LastMousePosition.y - ypos;
        m_PitchAngle += -deltaY * AngleChangeMul;
        m_PitchAngle = std::clamp<float>(m_PitchAngle, PitchAngleRange.x,PitchAngleRange.y);

        auto deltaX = m_LastMousePosition.x - xpos;
        m_YawAngle += deltaX * AngleChangeMul;
        
        updateCameraPosition();
    }

    m_LastMousePosition.x = xpos;
    m_LastMousePosition.y = ypos;
}

void modelviewer_window::updateCameraPosition() {
    auto pos = getPosition(m_PitchAngle, m_YawAngle, m_ZoomLevel);
    m_CameraPosition.x = pos.x;
    m_CameraPosition.y = pos.y;
    m_CameraPosition.z = pos.z;
}
