﻿#include <imgui/imgui.h>
#include "glm/glm.hpp"
#include "modelviewer_window.h"
#include "glfw/glfw3.h"
#include "regex"

using namespace modelViewer::res;
using namespace modelViewer::render;
using namespace modelViewer::common;

void modelviewer_window::onRender(float elapsed) {

    addNewModels();
	m_Renderer.render(m_Scene, m_Camera, true);
}


void modelviewer_window::addModel(modelViewer::res::model_info& info) {
    
    m_NewModelsQueue.push_back(info);
}


modelviewer_window::modelviewer_window(int width, int height, std::string title, bool fullscreen) : window(width,
                                                                                                           height,
                                                                                                           title,
                                                                                                           fullscreen)
                                                                                                           {
    
    setVsync(false);
    setTargetFrameRate(360);
    updateCameraPosition();
	//TODO set viewport when the window size changes too
	m_Camera.setViewPort(getWidth(), getHeight());
	m_Renderer.init(m_ObjectFactory);
    
    model_platform_info info;
    info.sizeZ = 12;
    info.sizeX = 12;
    info.lineSpace = 1;
	
	auto plane = m_Platform.generatePlane(m_ObjectFactory, info);
	auto grid = m_Platform.generateGrid(m_ObjectFactory, info);
	m_Scene.addStaticObject(plane);
	m_Scene.addStaticObject(grid);
}

modelviewer_window::~modelviewer_window() {
    
}

void modelviewer_window::setClearFlag(glm::vec4 color) {
    
	m_Renderer.setClearFlag(color);
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
	m_Camera.setPosition(pos);
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

	  if (ImGui::BeginMenu("clear"))
	  {
			if(ImGui::MenuItem("color"))
			{
				setClearMode(clear_mode::color);
			}
		  if(ImGui::MenuItem("skybox"))
		  {
			  setClearMode(clear_mode::skybox);
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
        m_ObjectFactory.getModelLoader().load(info.path, info);
        addModel(info);
    }
}


void modelviewer_window::addNewModels()
{
    //TODO the current imp is not efficient since it first loads the objects then applies limit, a better one should apply the limit when adding objects 
    for (auto& info: m_NewModelsQueue)
    {
		auto object = m_ObjectFactory.createObject(info);
		m_Scene.addModelObject(object);
    }

    m_NewModelsQueue.clear();
}
void modelviewer_window::openDemoModel(std::string name)
{
    model_info info;
    shader_asset_info fragShader { "res/shaders/sample/phong_phong_frag.glsl", shaderType::fragment};
    shader_asset_info vertShader { "res/shaders/sample/phong_phong_vert.glsl", shaderType::vertex};
    info.material.shaders.push_back(fragShader);
    info.material.shaders.push_back(vertShader);

    texture_asset_info textureAssetInfo;
    textureAssetInfo.paths.emplace_back("res/textures/Transparent.png");
    info.material.textures.push_back(textureAssetInfo);

    info.path = "res/models/primitives/" + name + ".fbx";
    info.name = name;
  
    addModel(info);
}

void modelviewer_window::setClearMode(modelViewer::render::clear_mode mode)
{
	m_Renderer.setClearMode(mode);
}
