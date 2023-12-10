﻿
#include "model_platform_buffer.h"


using namespace modelViewer::render;
using namespace modelViewer::res;

std::shared_ptr<mesh> generateGridMesh(const model_platform_info& info)
{
    auto positions = std::make_shared<std::vector<glm::vec3>>();
    auto colors = std::make_shared<std::vector<glm::vec4>>();
    glm::vec4 mainLineColor(1,1,1,0.3f);
    glm::vec4 sideLineColor(1,1,1,0.1f);
    
    glm::vec3 startPos(-info.sizeX/2 * info.lineSpace, -1, info.sizeX/2 * info.lineSpace);
    glm::vec3 currentPos = startPos;
    for (int i = 0; i < info.sizeX; ++i) {

        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z - (info.lineSpace * (info.sizeY - 1)));
        colors->emplace_back(mainLineColor);
        colors->emplace_back(mainLineColor);

        if (i == info.sizeX - 1)
            break;
        
        for (int j = 0; j < 10; ++j) {
            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z - (info.lineSpace * (info.sizeY - 1)));
            colors->emplace_back(sideLineColor);
            colors->emplace_back(sideLineColor);
            currentPos.x += info.lineSpace/10;
        }
    }

    currentPos = startPos;
    for (int i = 0; i < info.sizeY; ++i) {

        positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
        positions->emplace_back(currentPos.x + (info.lineSpace * (info.sizeX - 1)),currentPos.y, currentPos.z);
        colors->emplace_back(mainLineColor);
        colors->emplace_back(mainLineColor);

        if (i == info.sizeY - 1)
            break;
        
        for (int j = 0; j < 10; ++j) {

            positions->emplace_back(currentPos.x,currentPos.y, currentPos.z);
            positions->emplace_back(currentPos.x + (info.lineSpace * (info.sizeX - 1)),currentPos.y, currentPos.z);
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
    
    auto meshPtr = std::make_shared<mesh>(meshAsset);
    return meshPtr;
}

std::shared_ptr<mesh> generatePlaneMesh(const model_platform_info& info)
{
	auto positions = std::make_shared<std::vector<glm::vec3>>();
	auto normals = std::make_shared<std::vector<glm::vec3>>();
	
	float xWidth = info.sizeX * info.lineSpace;
	float zWidth = info.sizeY * info.lineSpace;
	
	float height = -1;

	positions->emplace_back(xWidth/2, height , -zWidth/2);
	positions->emplace_back(-xWidth/2, height , -zWidth/2);
	positions->emplace_back(-xWidth/2, height , zWidth/2);
	positions->emplace_back(xWidth/2, height , zWidth/2);

	for (int i = 0; i < 4; ++i) {
		normals->emplace_back(0, 1 , 0);
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

	auto meshPtr = std::make_shared<mesh>(meshAsset);
	return meshPtr;
}


std::shared_ptr<modelViewer::render::render_object> model_platform_buffer::generateGrid(shader_loader &shaderLoader,
	const model_platform_info &info)
{
	auto mesh = generateGridMesh(info);
	auto fragShaderAsset = shaderLoader.load(m_GridFragShaderPath, shaderType::fragment);
	auto vertexShaderAsset = shaderLoader.load(m_GridVertShaderPath, shaderType::vertex);
	shader fragShader (fragShaderAsset);
	fragShader.compile();
	fragShader.verify();
	
	shader vertexShader (vertexShaderAsset);
	vertexShader.compile();
	vertexShader.verify();
	
	auto program = std::make_shared<shader_program>(std::initializer_list<shader>{fragShader, vertexShader});
	program->validate();

	material_info materialInfo;
	std::vector<std::shared_ptr<texture>> textures;
	auto mat = std::make_shared<material>(materialInfo, textures, program);
	
	auto grid = std::make_shared<render_object>(mat, mesh, "platform_grid");
	grid->setRenderMode(render_mode::lines);
	grid->setCastShadows(false);
	grid->setReceiveShadows(false);
	
	return grid;
}

std::shared_ptr<modelViewer::render::render_object> model_platform_buffer::generatePlane(shader_loader &shaderLoader,
	const model_platform_info &info)
{
	auto mesh = generatePlaneMesh(info);
	auto fragShaderAsset = shaderLoader.load(m_PlaneFragShaderPath, shaderType::fragment);
	auto vertexShaderAsset = shaderLoader.load(m_PlaneVertShaderPath, shaderType::vertex);
	shader fragShader (fragShaderAsset);
	fragShader.compile();
	fragShader.verify();
	
	shader vertexShader (vertexShaderAsset);
	vertexShader.compile();
	vertexShader.verify();
	
	auto program = std::make_shared<shader_program>(std::initializer_list<shader>{fragShader, vertexShader});
	program->validate();

	material_info materialInfo;
	std::vector<std::shared_ptr<texture>> textures;
	auto mat = std::make_shared<material>(materialInfo, textures, program);

	auto plane = std::make_shared<render_object>(mat, mesh, "platform_plane");
	plane->setCastShadows(false);
	
	return plane;
}
