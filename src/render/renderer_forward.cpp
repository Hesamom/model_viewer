#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "renderer_forward.h"
#include "camera.h"
#include "object_factory.h"
#include "texture_setup.h"
#include "texture_2D.h"

using namespace modelViewer::res;
using namespace modelViewer::render;

void renderer_forward::render(render_scene& scene, camera& camera, bool shadowEnabled)
{
	if (shadowEnabled)
	{
		renderShadows(scene);
	}

	renderReflectionProbs(scene, camera);

	renderObjects(scene, camera, shadowEnabled);
}

void renderer_forward::renderDirectionalShadows(render_scene& scene) {

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "rendering directional shadows");
	
	glViewport(0, 0, SHADOW_DIR_WIDTH, SHADOW_DIR_HEIGHT);
	m_shadowBuffer.attachDepthTexture();
	glClear(GL_DEPTH_BUFFER_BIT);
	
	constexpr float near_plane = 30.0f;
	constexpr float far_plane = 100.0f;
	const auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	const auto lightView = glm::lookAt(scene.getDirectionalLight().direction * 50.0f,
	                        glm::vec3( 0.0f, 0.0f,  0.0f),
	                        glm::vec3( 0.0f, 1.0f,  0.0f));
	m_LightViewProjection = lightProjection * lightView;
	

	for (auto& object : scene.getObjects()) {

		if (!object->getCastShadows())
		{
			continue;
		}
		
		if (object->getMaterial()->getInfo().propertySet.renderQueue >= render_queue_transparent)
		{
			continue;
		}
		
		auto mvp = m_LightViewProjection * object->getTransform().getMatrix();
		m_shadowProgram->setUniform(m_MVPLocation, mvp);
		object->renderShadow();
	}

	glPopDebugGroup();
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

	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "rendering spot shadows");
	glViewport(0, 0, SHADOW_SPOT_WIDTH, SHADOW_SPOT_HEIGHT);
	
	int layer = 0;
	for (auto& spot : scene.getSpotLights()) {
		
		m_shadowBuffer.attachDepthTextureArray(layer);
		glClear(GL_DEPTH_BUFFER_BIT);
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
		
			if (object->getMaterial()->getInfo().propertySet.renderQueue >= render_queue_transparent)
			{
				continue;
			}
			
			auto mvp = spot.viewProjection * object->getTransform().getMatrix();
			m_shadowProgram->setUniform(m_MVPLocation, mvp);
			object->renderShadow();
		}
	}

	glPopDebugGroup();
}

void renderer_forward::renderShadows(render_scene& scene)
{
	m_shadowBuffer.bind();
	glCullFace(GL_FRONT);
	m_shadowProgram->bind();
	
	renderDirectionalShadows(scene);
	renderSpotShadows(scene);
	
	m_shadowBuffer.unbind();
}

const glm::vec3 cube_Map_Faces_Direction[] = {
	glm::vec3(1.0f, 0.0f, 0.0f),  // Positive X
	glm::vec3(-1.0f, 0.0f, 0.0f), // Negative X
	glm::vec3(0.0f, 1.0f, 0.0f),  // Positive Y
	glm::vec3(0.0f, -1.0f, 0.0f), // Negative Y
	glm::vec3(0.0f, 0.0f, 1.0f),  // Positive Z
	glm::vec3(0.0f, 0.0f, -1.0f)  // Negative Z
};

const glm::vec4 reflection_Debug_Colors[6] = {
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
};

void renderer_forward::renderReflectionProbs(render_scene& scene , camera& camera){
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "rendering reflection probs");

	m_reflectionBuffer.bind();
	glCullFace(GL_FRONT);
	glViewport(0, 0, REFLECTION_SIZE, REFLECTION_SIZE);

	for (unsigned int index = 0; index < 6; ++index) {
		m_reflectionBuffer.attachCubeMapFace(index);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(reflection_Debug_Colors[index].x, reflection_Debug_Colors[index].y, reflection_Debug_Colors[index].z, reflection_Debug_Colors[index].a);


		glm::mat4 viewMatrix = glm::lookAt(camera.getPosition(), camera.getPosition() + cube_Map_Faces_Direction[index], glm::vec3(0.0f, 1.0f, 0.0f));

		glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), 1.0f, 0.1f, 100.0f);

		for (auto& object : scene.getObjects()) {
			object->render(viewMatrix, projectionMatrix);
		}
	}

	m_reflectionBuffer.unbind();

	glPopDebugGroup();
}

bool compareRenderQueue(std::shared_ptr<render_object> o1, std::shared_ptr<render_object> o2)
{
	return o1->getMaterial()->getInfo().propertySet.renderQueue < o2->getMaterial()->getInfo().propertySet.renderQueue;
}

std::vector<std::shared_ptr<render_object>> renderer_forward::getSortedObjects(render_scene& scene)
{
	std::vector<std::shared_ptr<render_object>> objects;
	objects = scene.getObjects();
	if(m_ClearMode == clear_mode::skybox)
	{
		objects.push_back(m_Skybox);
	}
	std::sort(objects.begin(), objects.end(), compareRenderQueue);
	
	return objects;
}

void renderer_forward::renderObjects(render_scene& scene, camera& camera, bool shadowsEnabled)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "rendering objects");
	
	int viewportWidth, viewportHeight;
	camera.getViewport(viewportWidth, viewportHeight);
	
	glViewport(0, 0, viewportWidth, viewportHeight);
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearBufferfv(GL_COLOR, 0, &m_ClearFlag.x);
	glCullFace(GL_BACK);
	
	if(shadowsEnabled)
	{
		//TODO consider using another approach that would not be overriden by material, this is safe by now since we dont have 31 textures yet
		m_shadowBuffer.activateDepthMap(shadowmapDirSlot);
		m_shadowBuffer.activateDepthMapArray(shadowmapSpotSlot);
		m_EmptyShadowmap->active(emptyShadowmapSlot);
	}
	

	auto viewMatrix = camera.getView();
	auto projection = camera.getProjection();
	
	for (auto& object : getSortedObjects(scene))
	{
		if (shadowsEnabled)
		{
			if(object->getReceiveShadows())
			{
				object->getMaterial()->setShadowMapSlot(shadowmapDirSlot);
				object->getMaterial()->setSpotShadowMapSlot(shadowmapSpotSlot);
				object->getMaterial()->setLightViewProjection(m_LightViewProjection);
			}
			else
			{
				object->getMaterial()->setShadowMapSlot(emptyShadowmapSlot);
			}
		}
		
		//TODO implement light culling 
		object->setLight(scene.getDirectionalLight());
		object->getMaterial()->setPointLights(scene.getPointLights());
		object->getMaterial()->setSpotLights(scene.getSpotLights());
		
		object->render(viewMatrix, projection);
	}
	

	glPopDebugGroup();
}

void renderer_forward::setClearFlag(glm::vec4 color)
{
	m_ClearFlag = color;
}

void renderer_forward::init(object_factory& objectFactory)
{
	auto shaderLoader = objectFactory.getShaderLoader();
	auto vertShaderAsset = shaderLoader.load(m_DepthShaderVert, shaderType::vertex);
	auto fragShaderAsset = shaderLoader.load(m_DepthShaderFrag, shaderType::fragment);

	shader vertShadowShader(vertShaderAsset);
	shader fragShadowShader(fragShaderAsset);

	vertShadowShader.compile();
	vertShadowShader.verify();

	fragShadowShader.compile();
	fragShadowShader.verify();

	m_shadowProgram = std::make_unique<shader_program>(std::initializer_list<shader>{vertShadowShader, fragShadowShader});
	m_shadowProgram->validateLinking();

	m_shadowProgram->bind();
	m_MVPLocation = m_shadowProgram->getUniformLocation(m_MVPUniformName);

	m_shadowBuffer.bind();
	m_shadowBuffer.createDepthTexture(SHADOW_DIR_WIDTH, SHADOW_DIR_HEIGHT, true);
	m_shadowBuffer.createArrayDepthTexture(SHADOW_SPOT_WIDTH, SHADOW_SPOT_HEIGHT,SUPPORTTED_SPOT_LIGHTS, true);
	m_shadowBuffer.unbind();

	auto textureLoader = objectFactory.getTextureLoader();
	auto textureAsset = textureLoader.load(m_EmptyShadowmapTexture,3);
	texture_setup setup;
	setup.assets.push_back(textureAsset);

	m_EmptyShadowmap = std::make_shared<texture_2D>(setup);

	m_reflectionBuffer.bind();
	m_reflectionBuffer.createCubeMap(REFLECTION_SIZE);
	m_reflectionBuffer.unbind();


	createSkybox(objectFactory);
}

void renderer_forward::createSkybox(object_factory& objectFactory)
{
	model_info skyboxModel;
	shader_asset_info fragShader1 { "res/shaders/sample/skybox_frag.glsl", shaderType::fragment};
	shader_asset_info vertShader1 { "res/shaders/sample/skybox_vert.glsl", shaderType::vertex};
	skyboxModel.material.shaders.push_back(fragShader1);
	skyboxModel.material.shaders.push_back(vertShader1);


	texture_asset_info skyboxTexture;
	skyboxTexture.samplerName = "u_skybox";
	skyboxTexture.type = texture_asset_type::textureCube;
	skyboxTexture.paths.emplace_back("res/textures/skybox/sample/right.jpg");
	skyboxTexture.paths.emplace_back("res/textures/skybox/sample/left.jpg");
    skyboxTexture.paths.emplace_back("res/textures/skybox/sample/top.jpg");
    skyboxTexture.paths.emplace_back("res/textures/skybox/sample/bottom.jpg");
    skyboxTexture.paths.emplace_back("res/textures/skybox/sample/front.jpg");
	skyboxTexture.paths.emplace_back("res/textures/skybox/sample/back.jpg");
    skyboxTexture.forceFlip = false;
	skyboxModel.material.textures.push_back(skyboxTexture);
	skyboxModel.path = "res/models/primitives/cube.obj";
	skyboxModel.name = "skybox";
	skyboxModel.transform.setScale(glm::vec3(100.0));
	skyboxModel.material.propertySet.renderQueue = render_queue_transparent - 1;
	skyboxModel.material.propertySet.cullFaceEnabled = false;

	m_Skybox = objectFactory.createObject(skyboxModel);
}

void renderer_forward::setClearMode(clear_mode mode)
{
	m_ClearMode = mode;
}
