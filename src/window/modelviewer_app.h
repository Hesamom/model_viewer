
#ifndef MODEL_VIEWER_MODELVIEWER_WINDOW_H
#define MODEL_VIEWER_MODELVIEWER_WINDOW_H
#include "window_glfw.h"
#include "../resource/model_info.h"
#include "../render/render_scene.h"
#include "glm/glm.hpp"
#include "../render/model_platform_buffer.h"
#include "file_picker_windows.h"
#include "../render/object_factory.h"
#include "../render/renderer_forward.h"
#include "../render/gfx_device.h"


class modelviewer_app {
    
private:

	std::shared_ptr<window> m_Window;
	std::shared_ptr<modelViewer::render::gfx_device> m_Device;
	modelViewer::render::object_factory m_ObjectFactory;
    modelViewer::render::render_scene m_Scene;
    modelViewer::render::model_platform_buffer m_Platform;
	modelViewer::render::renderer_forward m_Renderer;
	modelViewer::render::camera m_Camera;
	
    file_picker_windows m_FilePicker {"select the model file"};
    std::vector<modelViewer::res::model_info> m_NewModelsQueue;
    bool m_IsMouseButtonDown = false;
    float m_PitchAngle = 0;
    float m_YawAngle = 0;
    float m_ZoomLevel = 10;
    bool m_isImGUIOpen = true;
    int m_LastLightId = 0;
    const glm::vec2 PitchAngleRange{-80,80};
    const float AngleChangeMul = 0.5f;
	int m_TargetFrameRate = -1;
	long double m_elapsedTimeSinceStart = 0;
    
            
    glm::vec<2,double,glm::defaultp> m_LastMousePosition;
	
    void addNewModels();
    void openDemoModel(std::string name);
    void updateCameraPosition();
	void displayFilePicker();
	void openModel(std::string path);
	modelViewer::res::model_info getDemoModel(const std::string& name) const;
	
	void displayMenubar();
	void openWallParallaxMap();
	void openWallNormalMap();
	void addSpotLight();
	void addPointLight();
	void addSpotLight(modelViewer::render::light_spot spot);
	void addPointLight(modelViewer::render::light_point point);

	static std::string label(std::string str, int id);

	void drawReflectionSettings();
	void drawSpotLightSettings();
	void drawPointLightSettings();
	void drawDirectionalLightSettings();
	void displayLightPanel();
    
protected:
    void onRender(float elapsed);
	void onSizeChanged(int height, int width);
	void onRenderImGUI();
	
    void onScrollChanged(double yOffset);
    void onMouseButtonChanged(int button, int action, int mods);
    void onMousePositionChanged(double xpos, double ypos);
	
public:
	 modelviewer_app(std::shared_ptr<window>& window, std::shared_ptr<modelViewer::render::gfx_device>& device);
     ~modelviewer_app();
	
	void addModel(modelViewer::res::model_info& info);
	void setClearMode(modelViewer::render::clear_mode mode);
	void loop();
	void openSpecularMapModel();
	int getTargetFrameRate();
	void setTargetFrameRate(int fps);
};


#endif //MODEL_VIEWER_MODELVIEWER_WINDOW_H
