﻿
#include <GL/glew.h>
#include <imgui/imgui.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "modelviewer_window.h"
#include "glfw/glfw3.h"
#include "regex"
#include "../render/material.h"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;

void modelviewer_window::onRender(float elapsed) {

    addNewModels();
  
    glClearBufferfv(GL_COLOR, 0, &m_ClearFlag.x);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    auto viewMatrix = glm::lookAt(
            m_CameraPosition, // Camera is at (4,3,3), in World Space
            glm::vec3(0,0,0), // and looks at the origin
            glm::vec3(0,1,0));  // Head is up (set to 0,-1,0 to look upside-down
            
    auto aspectRatio = (float)getWidth()/getHeight();
    auto projection = glm::perspective<float>(glm::radians(45.0),aspectRatio,0.1,100.0);
    
    //auto viewProjection = projection * viewMatrix;

    m_Platform.draw(viewMatrix, projection);
    for (auto& object : m_Scene.getObjects()) {

        object->render(viewMatrix, projection, render_mode::triangles);
    }
}


void modelviewer_window::addModel(modelViewer::res::model_info& info) {
    
    m_NewModelsQueue.push_back(info);
}


void verifyShader(shader& shader)
{
    shader.compile();
    if(!shader.isCompiled())
    {
        throw std::runtime_error("shader compilation failed: \n" + shader.getCompilationLog());
    }

    auto log = shader.getCompilationLog();
    if(!log.empty())
    {
        std::cout<< shader.getCompilationLog();
    }
}

std::shared_ptr<modelViewer::render::shader_program> modelviewer_window::getProgram(model_info &info) {

    std::vector<shader> shaders;
    for (auto& shaderInfo : info.material.shaders) {
        
        auto shaderAsset = m_ShaderLoader.load(shaderInfo.path, shaderInfo.type);
        shader shader(shaderAsset);
        shader.compile();
        verifyShader(shader);

        shaders.push_back(shader);
    }
    
    auto program = std::make_shared<shader_program>(shaders);
    if(!program->isLinked())
    {
        std::cerr<< program->getLinkLog() << std::endl;
    }

    return program;
}

std::shared_ptr<modelViewer::render::mesh> modelviewer_window::getMesh(model_info &info) {

    if (info.mesh)
    {
        return std::make_shared<mesh>(info.mesh);
    }
    
    info.mesh = m_MeshLoader.loadMesh(info.path);
    auto meshPtr  = std::make_shared<mesh>(info.mesh);
    
    return meshPtr;
}

std::vector<std::shared_ptr<modelViewer::render::texture>> modelviewer_window::getTextures(model_info &info) {

    std::vector<std::shared_ptr<modelViewer::render::texture>> textures;
    for (auto& textureInfo : info.material.textures ) {
        if (textureInfo.path.empty())
        {
            continue;
        }
        if (textureInfo.isNormal)
        {
            continue;
        }

        auto textureAsset = m_TextureLoader.load(textureInfo.path, 4);

        texture_setup setup;
        setup.asset = textureAsset;
        setup.isMipMapActive = true;
        setup.mipMapMaxLevel = 1000;
        setup.mipMapMinLevel = 0;
        //TODO set wrapping mode here

        auto texturePtr = std::make_shared<texture>(setup);
        textures.push_back(texturePtr);
    }
    
    return textures;
}

modelviewer_window::modelviewer_window(int width, int height, std::string title, bool fullscreen) : window(width,
                                                                                                           height,
                                                                                                           title,
                                                                                                           fullscreen)
                                                                                                           {
    
    setVsync(false);
    setTargetFrameRate(360);
    updateCameraPosition();
    
    model_platform_info info;
    info.sizeY = 12;
    info.sizeX = 12;
    info.lineSpace = 1;
    m_Platform.init(m_ShaderLoader, info);
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


void modelviewer_window::onRenderImGUI() {

    const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_None);
    ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, 0), ImGuiCond_None);

    ImGuiWindowFlags window_flags = 
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoResize | 
            ImGuiWindowFlags_NoCollapse | 
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoBackground;

    if (!ImGui::Begin("Title", &m_isImGUIOpen, window_flags))
    {
        ImGui::End();
        return;
    }

    //ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

    // Menu Bar
    if (ImGui::BeginMenuBar()) 
    {
        if (ImGui::BeginMenu("File"))
        {
            if(ImGui::MenuItem("Open"))
            {
                openModelFile();
            }
            ImGui::EndMenu();
        }
        

      if (ImGui::BeginMenu("Demo"))
      {
        
        std::string demoObjects[4] = {"cube","sphere","cylinder","plane"};

        for (auto& model : demoObjects)
        {
          if(ImGui::MenuItem(model.data()))
          {
            openDemoModel(model);
          }
        }
        ImGui::EndMenu();
      }

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void modelviewer_window::openModelFile() {
    
    std::string path;
    path.reserve(256);
    if(m_FilePicker.tryOpenPicker(path))
    {
        model_info info;
        info.path = path.c_str();
        info.name = path.c_str();
        m_MeshLoader.load(info.path, info);
        addModel(info);
    }
}


void modelviewer_window::addNewModels()
{
    //TODO the current imp is not efficient since it first loads the objects then applies limit, a better one should apply the limit when adding objects 
    for (auto& info: m_NewModelsQueue)
    {
        auto program = getProgram(info);
        auto mesh = getMesh(info);
        auto textures = getTextures(info);
        auto mat = std::make_shared<material>(info.material,textures, program);
        auto object = std::make_shared<render_object>(mat, mesh, info.name);
        
        object->setTransform(info.transform);
        m_Scene.addObject(object);
    }

    m_NewModelsQueue.clear();

    auto& objects = m_Scene.getObjects();
    if (objects.size() > MaxRenderingObjects)
    {
        int extraObjects =  objects.size() - MaxRenderingObjects;
        objects.erase(objects.begin(), objects.begin() + extraObjects);
    }
}
void modelviewer_window::openDemoModel(std::string name)
{
    model_info info;
    shader_asset_info fragShader { "res/shaders/sample/phong_phong_frag.glsl", shaderType::fragment};
    shader_asset_info vertShader { "res/shaders/sample/phong_phong_vert.glsl", shaderType::vertex};
    info.material.shaders.push_back(fragShader);
    info.material.shaders.push_back(vertShader);

    texture_asset_info textureAssetInfo;
    textureAssetInfo.path = "res/textures/Transparent.png";
    info.material.textures.push_back(textureAssetInfo);

    info.path = "res/models/primitives/" + name + ".fbx";
    info.name = name;
  
    addModel(info);
}
