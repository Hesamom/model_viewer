#include "src/resource/shader_loader.h"
#include "src/resource/shader_asset.h"
#include "src/resource/texture_loader.h"
#include "src/resource/mesh_loader.h"
#include "src/window/modelviewer_window.h"

int main()
{
    modelviewer_window window(1024, 1024, "Playground", false);
    window.setClearFlag(glm::vec4(0.15f,0.15f,0.15f,0));
    
    modelViewer::res::model_info info2;
    info2.fragmentShaderPath = "res/shaders/sample/sample_frag.glsl";
    info2.vertexShaderPath = "res/shaders/sample/static_test_vert.glsl";
    info2.texturePath = "res/textures/sample.png";
    info2.meshPath = "res/models/primitives/cube.fbx";
    info2.name = "cube";
    window.addModel(info2);
    
    window.draw();
    return 0;
}