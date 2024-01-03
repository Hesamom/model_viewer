#include "src/resource/shader_asset.h"
#include "src/window/modelviewer_window.h"


int main()
{
    using namespace modelViewer::res;
    modelviewer_window window(1024, 1024, "Playground", false);
    window.setClearFlag(glm::vec4(0.15f,0.15f,0.15f,0));

    model_info wallModel;
    shader_asset_info fragShader { "res/shaders/sample/phong_phong_parallex_mapping_frag.glsl", shaderType::fragment};
    shader_asset_info vertShader { "res/shaders/sample/phong_phong_normal_map_vert.glsl", shaderType::vertex};
    wallModel.material.shaders.push_back(fragShader);
    wallModel.material.shaders.push_back(vertShader);

    texture_asset_info diffuseTexture;
	diffuseTexture.type = texture_asset_type::texture2D;
	diffuseTexture.paths.emplace_back("res/textures/ParallexMapping/bricks2.jpg");
	diffuseTexture.samplerName = "u_diffuseSampler";
    wallModel.material.textures.push_back(diffuseTexture);

	texture_asset_info normalTexture;
	normalTexture.type = texture_asset_type::texture2D;
	normalTexture.paths.emplace_back("res/textures/ParallexMapping/bricks2_normal.jpg");
	normalTexture.samplerName = "u_normalSampler";
	wallModel.material.textures.push_back(normalTexture);

	texture_asset_info parallaxMappingTexture;
	parallaxMappingTexture.type = texture_asset_type::texture2D;
	parallaxMappingTexture.paths.emplace_back("res/textures/ParallexMapping/bricks2_disp.jpg");
	parallaxMappingTexture.samplerName = "u_depthSampler";
	parallaxMappingTexture.isHightMap = true;
	wallModel.material.textures.push_back(parallaxMappingTexture);

    wallModel.path = "res/models/primitives/plane.fbx";
    wallModel.name = "default model";
	wallModel.material.propertySet.colors.push_back({Literals::DiffuseAlbedo, glm::vec3 (1.0f)});
	wallModel.material.propertySet.colors.push_back({Literals::AmbientAlbedo, glm::vec3 (1.0f)});
	wallModel.material.propertySet.colors.push_back({Literals::SpecularAlbedo, glm::vec3 (0.2f)});
	wallModel.material.propertySet.floats.push_back({Literals::Shininess, 10});
	wallModel.material.propertySet.floats.push_back({Literals::Opacity, 1});
	wallModel.material.propertySet.cullFaceEnabled = false;
    window.addModel(wallModel);


    window.draw();
    return 0;
}