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
    shader_asset_info fragShader { "res/shaders/sample/phong_phong_normal_map_frag_2.glsl", shaderType::fragment};
    shader_asset_info vertShader { "res/shaders/sample/phong_phong_normal_map_vert_2.glsl", shaderType::vertex};
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
    window.addModel(wallModel);

    model_info skyboxModel;
    shader_asset_info fragShader1 { "res/shaders/sample/skybox_frag.glsl", shaderType::fragment};
    shader_asset_info vertShader1 { "res/shaders/sample/skybox_vert.glsl", shaderType::vertex};
    skyboxModel.material.shaders.push_back(fragShader1);
    skyboxModel.material.shaders.push_back(vertShader1);


    texture_asset_info skyboxTexture;
    skyboxTexture.type = texture_asset_type::cube;
    skyboxTexture.paths.emplace_back("res/textures/sky_right.jpg");
    skyboxTexture.paths.emplace_back("res/textures/sky_left.jpg");
    skyboxTexture.paths.emplace_back("res/textures/sky_top.jpg");
    skyboxTexture.paths.emplace_back("res/textures/sky_bottom.jpg");
    skyboxTexture.paths.emplace_back("res/textures/sky_back.jpg");
    skyboxTexture.paths.emplace_back("res/textures/sky_front.jpg");
    skyboxModel.material.textures.push_back(skyboxTexture);

    skyboxModel.path = "res/models/primitives/cube.fbx";
    skyboxModel.name = "skybox";
    skyboxModel.transform.setScale(glm::vec3(10.0));

    window.addModel(skyboxModel);

    window.draw();
    return 0;
}