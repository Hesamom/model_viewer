#include "src/resource/shader_loader.h"
#include "src/resource/shader_asset.h"
#include "src/resource/texture_loader.h"
#include "src/resource/mesh_loader.h"
#include "src/window/modelviewer_window.h"


int main()
{
    modelviewer_window window(1024, 1024, "Playground", false);
    window.setClearFlag(glm::vec4(0.15f,0.15f,0.15f,0));
    
    modelViewer::res::model_info info;
    info.fragmentShaderPath = "res/shaders/sample/phong_phong_frag.glsl";
    info.vertexShaderPath = "res/shaders/sample/phong_phong_vert.glsl";
    info.texturePath = "res/textures/Transparent.png";
    info.meshPath = "res/models/primitives/cube.fbx";
    info.name = "sphere";
    window.addModel(info);
    
    window.draw();
    return 0;
}