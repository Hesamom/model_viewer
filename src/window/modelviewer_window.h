
#ifndef MODEL_VIEWER_MODELVIEWER_WINDOW_H
#define MODEL_VIEWER_MODELVIEWER_WINDOW_H
#include "window.h"
#include "../resource/model_info.h"
#include "../resource/mesh_loader.h"
#include "../resource/texture_loader.h"
#include "../resource/shader_loader.h"
#include "../render/render_scene.h"
#include "glm/glm.hpp"

class modelviewer_window : public window {
    
private:
    modelViewer::res::mesh_loader m_MeshLoader;
    modelViewer::res::texture_loader m_TextureLoader;
    modelViewer::res::shader_loader m_ShaderLoader;
    modelViewer::render::render_scene m_Scene;
    glm::mat4 m_ViewProjection;
    std::vector<modelViewer::res::model_info> m_NewModelsQueue;
    glm::vec4 m_ClearFlag;
    glm::vec3 m_CameraPosition{0.0f,4.0f,5.0f};
    bool m_IsMouseButtonDown = false;
    float m_ViewAngle = 0;
    float m_ZoomLevel = m_CameraPosition.z;
    glm::vec<2,double,glm::defaultp> m_LastMousePosition;
    
    std::shared_ptr<modelViewer::render::shader_program> getProgram(modelViewer::res::model_info& info);
    std::shared_ptr<modelViewer::render::mesh> getMesh(modelViewer::res::model_info& info);
    std::shared_ptr<modelViewer::render::texture> getTexture(modelViewer::res::model_info& info);
    
protected:
    void onRender(float elapsed) override;
    void onScrollChanged(double yOffset) override;
    void onMouseButtonChanged(int button, int action, int mods) override;
    void onMousePositionChanged(double xpos, double ypos) override;
    
public:
    modelviewer_window(int width, int height, std::string title, bool fullscreen);
     ~modelviewer_window() override;
     void addModel(modelViewer::res::model_info& info);
     void setClearFlag(glm::vec4 color);
};


#endif //MODEL_VIEWER_MODELVIEWER_WINDOW_H
