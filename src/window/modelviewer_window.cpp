
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "modelviewer_window.h"
using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;

void modelviewer_window::onRender(float elapsed) {
    
    static float rotationSpeed = 2.0f;
    static float angle = 0;
    static const GLfloat green[] = { 0.0f, 0.25f, 0.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, green);
    
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
            glm::vec3(4,3,3), // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0));  // Head is up (set to 0,-1,0 to look upside-down
            
    auto projection = glm::perspective<float>(glm::radians(45.0),1,0.1,100.0);
    
    auto viewProjection = projection * viewMatrix;
    for (auto& object : m_Scene.getObjects()) {
        
        auto added = rotationSpeed * elapsed;
        angle += added;
        object->getTransform().setEularRotation(glm::vec3(angle,0,0));
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
    setTargetFrameRate(120);
}

modelviewer_window::~modelviewer_window() {
    
}
