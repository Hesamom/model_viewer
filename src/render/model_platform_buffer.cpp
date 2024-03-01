
#include "model_platform_buffer.h"
#include "object_factory.h"
#include "texture_setup.h"
#include "gl/texture_2D.h"

using namespace modelViewer::render;
using namespace modelViewer::res;

std::shared_ptr<mesh> generateGridMesh(const model_platform_info& info, gfx_device& device)
{
    auto positions = std::make_shared<std::vector<glm::vec3>>();
    auto colors = std::make_shared<std::vector<glm::vec4>>();
    glm::vec4 mainLineColor(1,1,1,0.5f);
    glm::vec4 sideLineColor(1,1,1,0.1f);
    
    glm::vec3 startPos(-info.sizeX/2 * info.lineSpace, 0, info.sizeX/2 * info.lineSpace);
    glm::vec3 currentPos = startPos;
    for (int i = 0; i < info.sizeX + 1; ++i) {

        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z - (info.lineSpace * (info.sizeZ)));
        colors->emplace_back(mainLineColor);
        colors->emplace_back(mainLineColor);

        if (i == info.sizeX)
            break;
        
        for (int j = 0; j < 10; ++j) {
            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z - (info.lineSpace * (info.sizeZ)));
            colors->emplace_back(sideLineColor);
            colors->emplace_back(sideLineColor);
            currentPos.x += info.lineSpace/10;
        }
    }

    currentPos = startPos;
    for (int i = 0; i < info.sizeZ + 1; ++i) {

        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
        positions->emplace_back(currentPos.x + (info.lineSpace * (info.sizeX)),currentPos.y, currentPos.z);
        colors->emplace_back(mainLineColor);
        colors->emplace_back(mainLineColor);

        if (i == info.sizeZ)
            break;
        
        for (int j = 0; j < 10; ++j) {

            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
            positions->emplace_back(currentPos.x + (info.lineSpace * (info.sizeX)),currentPos.y, currentPos.z);
            colors->emplace_back(sideLineColor);
            colors->emplace_back(sideLineColor);
            currentPos.z -= info.lineSpace/10;
        }
    }

    auto indices = std::make_shared<std::vector<unsigned int>>();
    unsigned int index = 0;
    for (int i = 0; i < positions->size() ; ++i) {
        indices->push_back(index);
        indices->push_back(index + 1);
        index += 2;
    }
    
    auto meshAsset = std::make_shared<mesh_asset>();
    meshAsset->positions = positions;
    meshAsset->indices = indices;
    meshAsset->colors = colors;
	meshAsset->name = "grid";
	
    auto meshPtr = device.createMesh(meshAsset);
    return meshPtr;
}

std::shared_ptr<mesh> generatePlaneMesh(const model_platform_info& info, gfx_device& device)
{
	auto positions = std::make_shared<std::vector<glm::vec3>>();
	auto normals = std::make_shared<std::vector<glm::vec3>>();
	
	float xWidth = info.sizeX * info.lineSpace;
	float zWidth = info.sizeZ * info.lineSpace;
	
	float height = 0;

	positions->emplace_back(xWidth/2, height , -zWidth/2);
	positions->emplace_back(-xWidth/2, height , -zWidth/2);
	positions->emplace_back(-xWidth/2, height , zWidth/2);
	positions->emplace_back(xWidth/2, height , zWidth/2);

	for (int i = 0; i < 4; ++i) {
		normals->emplace_back(0, 1, 0);
	}
	
	
	auto indices = std::make_shared<std::vector<unsigned int>>();
	indices->emplace_back(0);
	indices->emplace_back(1);
	indices->emplace_back(2);

	indices->emplace_back(0);
	indices->emplace_back(2);
	indices->emplace_back(3);
	
	auto meshAsset = std::make_shared<mesh_asset>();
	meshAsset->positions = positions;
	meshAsset->indices = indices;
	meshAsset->normals = normals;
	meshAsset->name = "plane";

	auto meshPtr = device.createMesh(meshAsset);
	return meshPtr;
}


std::shared_ptr<object_renderer> model_platform_buffer::generateGrid(object_factory& objectFactory,
                                                                     const model_platform_info &info, gfx_device& device)
{
	auto shaderLoader = objectFactory.getShaderLoader();
	auto mesh = generateGridMesh(info, device);
	auto fragShaderAsset = shaderLoader.load(m_GridFragShaderPath, shaderType::fragment);
	auto vertexShaderAsset = shaderLoader.load(m_GridVertShaderPath, shaderType::vertex);
	shader_gl fragShader (fragShaderAsset);
	fragShader.compile();
	fragShader.verify();
	
	shader_gl vertexShader (vertexShaderAsset);
	vertexShader.compile();
	vertexShader.verify();
	
	auto program = std::make_shared<shader_program_gl>(std::initializer_list<shader_gl>{fragShader, vertexShader});
    program->validateLinking();

	material_asset materialInfo;
	std::vector<texture_binding> textures;
	materialInfo.propertySet.floats.push_back({Literals::Opacity, 0.5f});
	materialInfo.propertySet.renderQueue = (render_queue_transparent + 1);

	auto defaults = objectFactory.getDefaultTextures();
	auto mat = std::make_shared<material>(materialInfo, textures, program, defaults);
	auto grid = std::make_shared<object_renderer>(mat, mesh, "platform_grid");
	
	grid->setRenderMode(render_mode::lines);
	grid->setCastShadow(false);
	grid->setReceiveShadows(false);
	grid->setCastReflection(false);
	
	return grid;
}

std::shared_ptr<object_renderer> model_platform_buffer::generatePlane(object_factory& objectFactory,
                                                                      const model_platform_info &info, gfx_device& device)
{
	auto shaderLoader = objectFactory.getShaderLoader();
	auto mesh = generatePlaneMesh(info, device);
	auto fragShaderAsset = shaderLoader.load(m_PlaneFragShaderPath, shaderType::fragment);
	auto vertexShaderAsset = shaderLoader.load(m_PlaneVertShaderPath, shaderType::vertex);
	shader_gl fragShader (fragShaderAsset);
	fragShader.compile();
	fragShader.verify();
	
	shader_gl vertexShader (vertexShaderAsset);
	vertexShader.compile();
	vertexShader.verify();
	
	auto program = std::make_shared<shader_program_gl>(std::initializer_list<shader_gl>{fragShader, vertexShader});
    program->validateLinking();


	auto diffuseTextureAsset = objectFactory.getTextureLoader().load(m_PlaneDiffuseTexture,3);
	texture_setup textureSetup;
	textureSetup.assets.push_back(diffuseTextureAsset);
	auto diffuseTexture = std::make_shared<texture_2D>(textureSetup);
	
	std::vector<texture_binding> textures;
	texture_binding binding;
	binding.texture =  diffuseTexture;
	binding.samplerName = "diffuseSampler";
	textures.push_back(binding);
	
	material_asset materialInfo;
	materialInfo.propertySet.colors.push_back({Literals::SpecularAlbedo, glm::vec3 (0)});
	materialInfo.propertySet.colors.push_back({Literals::DiffuseAlbedo, Literals::DefaultDiffuseAlbedo});
	materialInfo.propertySet.colors.push_back({Literals::AmbientAlbedo, Literals::DefaultAmbientAlbedo});
	materialInfo.propertySet.floats.push_back({Literals::Opacity, 1.0f});
	materialInfo.propertySet.floats.push_back({Literals::Shininess, 1});
	materialInfo.propertySet.floats.push_back({Literals::Reflectivity, 0.3f});
	materialInfo.propertySet.renderQueue = render_queue_transparent;
	auto defaults = objectFactory.getDefaultTextures();
	auto mat = std::make_shared<material>(materialInfo, textures, program, defaults);

	auto plane = std::make_shared<object_renderer>(mat, mesh, "platform_plane");
	plane->setCastShadow(false);
	plane->setReceiveShadows(true);
	plane->setCastReflection(false);
	plane->setReflectionMode(reflection_mode::environment);
	
	return plane;
}
