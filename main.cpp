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
    
    texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::diffuse;
	diffuseTexture.path = "res/textures/wall.jpg";
    info.material.textures.push_back(diffuseTexture);

	texture_asset_info normalTexture;
	normalTexture.type = texture_asset_type::normal;
	normalTexture.path = "res/textures/normal_map_wall.png";
	info.material.textures.push_back(normalTexture);
    
    info.path = "res/models/primitives/plane.fbx";
    info.name = "default model";
	info.material.propertySet.diffuseAlbedo = glm::vec3 (1.0f);
	info.material.propertySet.specularAlbedo = glm::vec3 (0.2f);
	info.material.propertySet.shininess = 10;
    window.addModel(info);
    
    window.draw();
    return 0;
}