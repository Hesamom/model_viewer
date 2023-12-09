﻿
#ifndef MODEL_VIEWER_MODELVIEWER_WINDOW_H
#define MODEL_VIEWER_MODELVIEWER_WINDOW_H
#include "window.h"
#include "../resource/model_info.h"
#include "../render/render_scene.h"
#include "glm/glm.hpp"
#include "../render/model_platform_buffer.h"
#include "file_picker_windows.h"
#include "../render/object_factory.h"

class modelviewer_window : public window {
    
private:
    
	modelViewer::render::object_factory m_ObjectFactory;
    modelViewer::render::render_scene m_Scene;
    modelViewer::render::model_platform_buffer m_Platform;
    file_picker_windows m_FilePicker {"select the model file"};
    glm::mat4 m_ViewProjection;
    std::vector<modelViewer::res::model_info> m_NewModelsQueue;
    glm::vec4 m_ClearFlag;
    glm::vec3 m_CameraPosition{0.0f,4.0f,5.0f};
    bool m_IsMouseButtonDown = false;
    float m_PitchAngle = 0;
    float m_YawAngle = 0;
    float m_ZoomLevel = 5;
    bool m_isImGUIOpen = true;
    const int MaxRenderingObjects = 1;
    
    const glm::vec2 PitchAngleRange{-80,80};
    const float AngleChangeMul = 0.5f;
    
            
    glm::vec<2,double,glm::defaultp> m_LastMousePosition;
	
    void addNewModels();
    void openDemoModel(std::string name);
    void updateCameraPosition();
    void openModelFile();
    
protected:
    void onRender(float elapsed) override;
    void onScrollChanged(double yOffset) override;
    void onMouseButtonChanged(int button, int action, int mods) override;
    void onMousePositionChanged(double xpos, double ypos) override;
    void onRenderImGUI() override;
    
public:
    modelviewer_window(int width, int height, std::string title, bool fullscreen);
     ~modelviewer_window() override;
     void addModel(modelViewer::res::model_info& info);
     void setClearFlag(glm::vec4 color);
};


#endif //MODEL_VIEWER_MODELVIEWER_WINDOW_H
