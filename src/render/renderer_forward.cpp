#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "renderer_forward.h"
#include "camera.h"
#include "object_factory.h"
#include "texture_setup.h"
#include "../resource/shader_asset.h"

using namespace modelViewer::res;
using namespace modelViewer::render;

const glm::vec3 m_CubeMapFacesDirection[] = {
	glm::vec3(1.0f, 0.0f, 0.0f),  // Positive X
	glm::vec3(-1.0f, 0.0f, 0.0f), // Negative X
	glm::vec3(0.0f, 1.0f, 0.0f),  // Positive Y
	glm::vec3(0.0f, -1.0f, 0.0f), // Negative Y
	glm::vec3(0.0f, 0.0f, 1.0f),  // Positive Z
	glm::vec3(0.0f, 0.0f, -1.0f)  // Negative Z
};

const std::string m_SideNames[]
	{
		"x+", "x-","y+","y-","z+","z-"
	};

void renderer_forward::render(render_scene& scene, camera& camera, bool shadowEnabled, bool reflectionEnabled)
{
	if (shadowEnabled)
	{
		renderShadows(scene);
	}

	if (reflectionEnabled)
	{
		renderReflectionMap(scene, camera);
	}

	renderObjects(scene, camera, shadowEnabled, reflectionEnabled);
}

void renderer_forward::renderReflectionMap(render_scene& scene , camera& camera){

	if (!requiresReflectionMapUpdate(scene,camera))
	{
		return;
	}

	m_Device->pushDebugGroup("rendering reflection map");

	m_ReflectionBuffer->bind();
	
	//m_Device->setCullFaceMode(cull_face_mode::front);
	
	
	m_Device->setViewport(REFLECTION_SIZE,REFLECTION_SIZE);
	m_ReflectionBuffer->attachDepthTexture();
	m_EmptyReflectionMap->active(reflectionMapSlot);
	
	auto meshes = getSortedObjects(scene, m_ReflectionClearMode == clear_mode::skybox);
	const glm::vec3  up (0.01f, -1.0f, 0.01f);
	glm::mat4 projection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 500.0f);
	
	for (int index = 0; index < 6; ++index) {

		m_Device->pushDebugGroup(("side: " + m_SideNames[index]).c_str());
		m_ReflectionBuffer->attachCubeMapFace(index);

		if (m_ReflectionClearMode == clear_mode::color)
		{
			m_ReflectionBuffer->clearColorBuffer(m_ReflectionClearFlag);
		}

		m_ReflectionBuffer->clearDepthBuffer();
		
		glm::mat4 view = glm::lookAt(m_ReflectionPosition, m_ReflectionPosition + m_CubeMapFacesDirection[index], up);
		
		for (auto& mesh : meshes) {
			
			if (mesh->getCastReflection() && mesh->getReflectionMode() == reflection_mode::disabled)
			{
				//TODO we can use a simple lit shader such as gouraud instead of the object's original shader to improve performance
				mesh->render(view, projection);
			}
		}
		
		m_Device->popDebugGroup();
	}

	m_ReflectionBuffer->unbind();
	m_Device->popDebugGroup();
}

void renderer_forward::renderDirectionalShadows(render_scene& scene) {

	m_Device->pushDebugGroup("rendering directional shadows");
	m_Device->setViewport(SHADOW_DIR_WIDTH, SHADOW_DIR_HEIGHT);
	m_shadowBuffer->attachDepthTexture();
	m_shadowBuffer->clearDepthBuffer();
	
	constexpr float near_plane = 30.0f;
	constexpr float far_plane = 100.0f;
	const auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	const auto lightView = glm::lookAt(scene.getDirectionalLight().direction * 50.0f,
	                        glm::vec3( 0.0f, 0.0f,  0.0f),
	                        glm::vec3( 0.0f, 1.0f,  0.0f));
	m_LightViewProjection = lightProjection * lightView;
	

	//TODO do we need sorting here? 
	for (auto& object : scene.getObjects()) {

		if (!object->getCastShadows())
		{
			continue;
		}

		for (auto& meshRenderer : object->getRenderers()) {
			
			if (meshRenderer->getMaterial()->getInfo().propertySet.renderQueue >= render_queue_transparent)
			{
				continue;
			}

			auto mvp = m_LightViewProjection * object->getTransform().getMatrix();
			m_shadowProgram->setUniform(m_MVPUniformName, mvp, true);
			meshRenderer->renderShadow();
		}
	}

	m_Device->popDebugGroup();
}

glm::mat4 compute_view_matrix_for_rotation(glm::vec3 origin, glm::vec3 rot) {
	glm::mat4 mat(1.0);
	float rx = glm::radians(rot.x);
	float ry = glm::radians(rot.y);
	float rz = glm::radians(rot.z);
	mat = glm::rotate(mat, -rx, glm::vec3(1, 0, 0));
	mat = glm::rotate(mat, -ry, glm::vec3(0, 1, 0));
	mat = glm::rotate(mat, -rz, glm::vec3(0, 0, 1));
	mat = glm::translate(mat, -origin);
	return mat;
}

void renderer_forward::renderSpotShadows(render_scene& scene) {

	m_Device->pushDebugGroup( "rendering spot shadows");
	m_Device->setViewport(SHADOW_SPOT_WIDTH, SHADOW_SPOT_HEIGHT);
	
	int layer = 0;
	for (auto& spot : scene.getSpotLights()) {
		
		m_shadowBuffer->attachDepthTextureArray(layer);
		m_shadowBuffer->clearDepthBuffer();
		layer++;
		
		constexpr float near_plane = 0.1f;
		constexpr float far_plane = 100.0f;
		auto lightProjection = glm::perspective(glm::radians(spot.outerCutoff), 1.0f, near_plane, far_plane);
		const auto lightView = glm::lookAt(spot.position,
							spot.position + (spot.direction * 10.0f),
							glm::vec3( 0.0f, 1.0f,  0.0f));
		spot.viewProjection =  lightProjection * lightView;

		for (auto& object : scene.getObjects()) {
			
			if (!object->getCastShadows())
			{
				continue;
			}

			//TODO remove duplication with directional light method 
			for (auto& meshRenderer : object->getRenderers()) {

				if (meshRenderer->getMaterial()->getInfo().propertySet.renderQueue >= render_queue_transparent)
				{
					continue;
				}

				auto mvp = spot.viewProjection * object->getTransform().getMatrix();
				m_shadowProgram->setUniform(m_MVPUniformName, mvp, true);
				meshRenderer->renderShadow();
			}
		}
	}

	m_Device->popDebugGroup();
}

void renderer_forward::renderShadows(render_scene& scene)
{
	m_shadowBuffer->bind();
	//m_Device->setCullFaceMode(cull_face_mode::front);
	m_shadowProgram->bind();
	
	renderDirectionalShadows(scene);
	renderSpotShadows(scene);
	
	m_shadowBuffer->unbind();
}

bool compareRenderQueue(std::shared_ptr<mesh_renderer> o1, std::shared_ptr<mesh_renderer> o2)
{
	return o1->getMaterial()->getInfo().propertySet.renderQueue < o2->getMaterial()->getInfo().propertySet.renderQueue;
}

std::vector<std::shared_ptr<mesh_renderer>> renderer_forward::getSortedObjects(render_scene& scene, bool includeSkybox)
{
	std::vector<std::shared_ptr<mesh_renderer>> renderers;
	for (auto& object : scene.getObjects()) {
		for (auto& mesh : object->getRenderers()) {
			renderers.push_back(mesh);
		}
	}

	if(includeSkybox)
	{
		renderers.push_back(m_Skybox->getRenderers().at(0));
	}
	
	std::sort(renderers.begin(), renderers.end(), compareRenderQueue);
	
	return renderers;
}

void renderer_forward::renderObjects(render_scene& scene, camera& camera, bool shadowsEnabled, bool reflectionEnabled)
{
	m_Device->pushDebugGroup("rendering objects");
	
	int viewportWidth, viewportHeight;
	camera.getViewport(viewportWidth, viewportHeight);
	m_Device->setViewport(viewportWidth, viewportHeight);
	m_Device->clearDepthBuffer();

	if (m_ClearMode == clear_mode::color)
	{
		m_Device->clearColorBuffer(m_ClearFlag);
	}

	//m_Device->setCullFaceMode(cull_face_mode::back);
	
	if (shadowsEnabled)
	{
		//TODO consider using another approach that would not be overriden by material, this is safe by now since we dont have 31 textures yet
		m_shadowBuffer->activateDepthMap(shadowmapDirSlot);
		m_shadowBuffer->activateDepthMapArray(shadowmapSpotSlot);
		m_EmptyShadowmap->active(emptyShadowmapSlot);
	}

	if (reflectionEnabled)
	{
		m_SkyboxCubeMap->active(skyboxReflectionMapSlot);
		m_ReflectionBuffer->activateCubeMap(reflectionMapSlot);
		m_EmptyReflectionMap->active(emptyReflectionMapSlot);
	}
	

	auto viewMatrix = camera.getView();
	auto projection = camera.getProjection();
	
	for (auto& renderer : getSortedObjects(scene, m_ClearMode == clear_mode::skybox))
	{
		if (shadowsEnabled)
		{
			if (renderer->getReceiveShadows())
			{
				renderer->getMaterial()->setShadowMapSlot(shadowmapDirSlot);
				renderer->getMaterial()->setSpotShadowMapSlot(shadowmapSpotSlot);
				renderer->getMaterial()->setLightViewProjection(m_LightViewProjection);
			}
			else
			{
				renderer->getMaterial()->setShadowMapSlot(emptyShadowmapSlot);
				renderer->getMaterial()->setSpotShadowMapSlot(shadowmapSpotSlot);
			}
		}
		
		if (reflectionEnabled && renderer->getMaterial()->isReflective())
		{
			auto mode = renderer->getReflectionMode();
			switch (mode) {
				case reflection_mode::environment:
					renderer->getMaterial()->setReflectionMapSlot(reflectionMapSlot);
					break;
				case  reflection_mode::skybox:
					renderer->getMaterial()->setReflectionMapSlot(skyboxReflectionMapSlot);
					break;
				case reflection_mode::disabled:
					renderer->getMaterial()->setReflectionMapSlot(emptyReflectionMapSlot);
					
			}
		}
		
		//TODO implement light culling 
		renderer->getMaterial()->setCameraPosition(camera.getPosition());
		renderer->getMaterial()->setDirectionalLight(scene.getDirectionalLight());
		renderer->getMaterial()->setPointLights(scene.getPointLights());
		renderer->getMaterial()->setSpotLights(scene.getSpotLights());

		m_Device->pushDebugGroup(renderer->getName().c_str());
		renderer->render(viewMatrix, projection);
		m_Device->popDebugGroup();
	}
	

	m_Device->popDebugGroup();
}

renderer_forward::renderer_forward(const std::shared_ptr<gfx_device>& device, object_factory& factory) {
	m_Device = device;

	auto shaderLoader = factory.getShaderLoader();
	
	initShadowmap(factory, shaderLoader);
	initReflectionMap(factory);
	initSkybox(factory);
}

void renderer_forward::setClearFlag(glm::vec4 color)
{
	m_ClearFlag = color;
}

void renderer_forward::initShadowmap(object_factory& objectFactory, shader_loader& shaderLoader)
{
	std::vector<std::shared_ptr<shader_asset>> assets;
	assets.emplace_back(shaderLoader.load(m_DepthShaderVert, shaderType::vertex));
	assets.emplace_back(shaderLoader.load(m_DepthShaderFrag, shaderType::fragment));;
	
	m_shadowProgram = m_Device->createProgram(assets);
	m_shadowProgram->bind();

	auto shadowDirName = std::string("shadowmap_dir");
	m_shadowBuffer = m_Device->createFramebuffer();
	m_shadowBuffer->bind();
	m_shadowBuffer->createDepthTexture(SHADOW_DIR_WIDTH, SHADOW_DIR_HEIGHT, true, shadowDirName);
	m_shadowBuffer->createArrayDepthTexture(SHADOW_SPOT_WIDTH, SHADOW_SPOT_HEIGHT,SUPPORTED_SPOT_LIGHTS, true);
	m_shadowBuffer->unbind();

	auto textureLoader = objectFactory.getTextureLoader();
	auto textureAsset = textureLoader.load(m_EmptyShadowmapTexture,3);
	texture_setup setup;
	setup.assets.push_back(textureAsset);
	
	m_EmptyShadowmap = m_Device->createTexture2D(setup);
}

void renderer_forward::initReflectionMap(object_factory& objectFactory)
{
	auto reflectionMapName = std::string("reflection_cubeMap");
	auto reflectionMapDepthName = std::string("reflection_depth");

	m_ReflectionBuffer = m_Device->createFramebuffer();
	m_ReflectionBuffer->bind();
	m_ReflectionBuffer->createCubeMap(REFLECTION_SIZE, reflectionMapName);
	m_ReflectionBuffer->createDepthTexture(REFLECTION_SIZE, REFLECTION_SIZE, false, reflectionMapDepthName);
	m_ReflectionBuffer->unbind();
	
	texture_setup setup;
	auto textureAsset = objectFactory.getTextureLoader().load(m_EmptyReflectionTexturePath, 4, false);
	for (int i = 0; i < 6; ++i) {
		setup.assets.emplace_back(textureAsset);
	}
	setup.type = res::texture_asset_type::textureCube;
	m_EmptyReflectionMap = m_Device->createTextureCube(setup);
}


void renderer_forward::initSkybox(object_factory& objectFactory)
{
	model_info skyboxModel;
	shader_asset_info fragShader1 { literals::shaders::skybox_frag, shaderType::fragment};
	shader_asset_info vertShader1 {  literals::shaders::skybox_vert, shaderType::vertex};
	
	auto material = std::make_shared<material_asset>();

	material->shaders.push_back(fragShader1);
	material->shaders.push_back(vertShader1);


	texture_asset_info skyboxTexture;
	skyboxTexture.samplerName = "u_skybox";
	skyboxTexture.type = texture_asset_type::textureCube;
	skyboxTexture.paths.emplace_back(literals::textures::skybox_right);
	skyboxTexture.paths.emplace_back(literals::textures::skybox_left);
    skyboxTexture.paths.emplace_back(literals::textures::skybox_top);
    skyboxTexture.paths.emplace_back(literals::textures::skybox_bottom);
    skyboxTexture.paths.emplace_back(literals::textures::skybox_front);
	skyboxTexture.paths.emplace_back(literals::textures::skybox_back);
    skyboxTexture.forceFlip = false;
	material->textures.push_back(skyboxTexture);
	skyboxModel.path =  literals::models::primitive_cube;
	skyboxModel.name = "skybox";
	skyboxModel.transform.setScale(glm::vec3(100.0));
	material->propertySet.renderQueue = render_queue_transparent - 1;
	material->propertySet.cullFaceMode = cull_face_mode::disabled;

	std::vector<std::shared_ptr<material_asset>> materials;
	materials.push_back(material);

	skyboxModel.materials = materials;
	m_Skybox = objectFactory.createObject(skyboxModel);
	m_SkyboxCubeMap = m_Skybox->getRenderers().at(0)->getMaterial()->getBoundTextures().at(0);
	m_Skybox->setCastReflection(true);
	m_Skybox->setReflectionMode(reflection_mode::disabled);
}

void renderer_forward::setClearMode(clear_mode mode)
{
	m_ClearMode = mode;
}

void renderer_forward::setReflectionPosition(const glm::vec3& pos)
{
	m_ReflectionPosition = pos;
}

void renderer_forward::setReflectionClearFlag(const glm::vec4& color)
{
	m_ReflectionClearFlag = color;
}

renderer_forward::~renderer_forward() {
	
}

glm::vec3& renderer_forward::getReflectionPosition()
{
	return m_ReflectionPosition;
}

//TODO we should receive a frustum culled set of objects but we don't have that filter yet so 
bool renderer_forward::requiresReflectionMapUpdate(render_scene& scene, camera& camera)
{
	bool containsReflectiveRenderer = false;
	bool containsReflectionCastingRenderer = false;
	
	for (auto& object : scene.getObjects()) {
		if (!containsReflectiveRenderer && object->getReflectionMode() == reflection_mode::environment)
		{
			containsReflectiveRenderer = true;
		}
		if (!containsReflectionCastingRenderer && object->getCastReflection())
		{
			containsReflectionCastingRenderer = true;
		}
		if (containsReflectiveRenderer && containsReflectionCastingRenderer)
		{
			return true;
		}
	}
	
	return containsReflectiveRenderer && containsReflectionCastingRenderer;
}

void renderer_forward::setReflectionClearMode(clear_mode mode)
{
	m_ReflectionClearMode = mode;
}
