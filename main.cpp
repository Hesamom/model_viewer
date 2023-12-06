#include "src/resource/shader_loader.h"
#include "src/resource/shader_asset.h"
#include "src/resource/texture_loader.h"
#include "src/resource/model_loader.h"
#include "src/window/modelviewer_window.h"


int main()
{
    using namespace modelViewer::res;
    modelviewer_window window(1024, 1024, "Playground", false);
    window.setClearFlag(glm::vec4(0.15f,0.15f,0.15f,0));
    
    model_info info;
    shader_asset_info fragShader { "res/shaders/sample/phong_phong_normal_map_frag.glsl", shaderType::fragment};
    shader_asset_info vertShader { "res/shaders/sample/phong_phong_normal_map_vert.glsl", shaderType::vertex};
    info.material.shaders.push_back(fragShader);
    info.material.shaders.push_back(vertShader);
    
    texture_asset_info textureAssetInfo;
    textureAssetInfo.type = texture_asset_type::diffuse;
    textureAssetInfo.path = "res/textures/sample.png";
    info.material.textures.push_back(textureAssetInfo);
    
    info.path = "res/models/complex/monkey.fbx";
    info.name = "default model";
    window.addModel(info);
    
    window.draw();
    return 0;
}