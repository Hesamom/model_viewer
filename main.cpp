#include "src/resource/shader_asset.h"
#include "src/resource/texture_loader.h"
#include "src/resource/model_loader.h"
#include "src/window/modelviewer_window.h"


int main()
{
    using namespace modelViewer::res;
    modelviewer_window window(1024, 1024, "Playground", false);
    window.setClearFlag(glm::vec4(0.15f,0.15f,0.15f,0));

    model_info wallModel;
    shader_asset_info fragShader { "res/shaders/sample/phong_phong_normal_map_frag.glsl", shaderType::fragment};
    shader_asset_info vertShader { "res/shaders/sample/phong_phong_normal_map_vert.glsl", shaderType::vertex};
    wallModel.material.shaders.push_back(fragShader);
    wallModel.material.shaders.push_back(vertShader);

    texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::diffuse;
	diffuseTexture.paths.emplace_back("res/textures/wall.jpg");
    wallModel.material.textures.push_back(diffuseTexture);

	texture_asset_info normalTexture;
	normalTexture.type = texture_asset_type::normal;
	normalTexture.paths.emplace_back("res/textures/normal_map_wall.png");
	wallModel.material.textures.push_back(normalTexture);

    wallModel.path = "res/models/primitives/plane.fbx";
    wallModel.name = "default model";
    wallModel.material.propertySet.diffuseAlbedo = glm::vec3 (1.0f);
    wallModel.material.propertySet.specularAlbedo = glm::vec3 (0.2f);
    wallModel.material.propertySet.shininess = 10;
	wallModel.material.propertySet.cullFaceEnabled = false;
    window.addModel(wallModel);


    window.draw();
    return 0;
}