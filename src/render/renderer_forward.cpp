
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "renderer_forward.h"
#include "camera.h"
#include "object_factory.h"

using namespace modelViewer::res;
using namespace modelViewer::render;

void renderer_forward::render(render_scene& scene, camera& camera, bool shadowEnabled)
{
	if (shadowEnabled)
	{
		renderShadows(scene, camera);
	}

	renderObjects(scene, camera, shadowEnabled);
}

void renderer_forward::renderShadows(modelViewer::render::render_scene& scene, camera& camera)
{
	glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0, -1, "rendering shadows");
	
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	m_shadowBuffer.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	float near_plane = 30.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(scene.getLight().position,
		glm::vec3( 0.0f, 0.0f,  0.0f),
		glm::vec3( 0.0f, 1.0f,  0.0f));
	m_LightViewProjection = lightProjection * lightView;
	
	m_shadowProgram->bind();
	for (auto& object : scene.getObjects()) {

		if (!object->getCastShadows())
		{
			continue;
		}
		
		//currently transparent objects can not cast shadows 
		if (object->getMaterial()->getInfo().propertySet.renderQueue >= render_queue_transparent)
		{
			continue;
		}


		auto mvp = m_LightViewProjection * object->getTransform().getMatrix();
		m_shadowProgram->setUniform(m_MVPLocation, mvp);
		object->renderShadow();
	}
	
	m_shadowBuffer.unbind();
	
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
		m_shadowBuffer.activateDepthMap(shadowmapSlot);
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
				object->getMaterial()->setShadowMapSlot(shadowmapSlot);
				object->getMaterial()->setLightViewProjection(m_LightViewProjection);
			}
			else
			{
				object->getMaterial()->setShadowMapSlot(emptyShadowmapSlot);
			}
		}
		
		object->setLight(scene.getLight());
		//TODO compare the point light ray with an object's AABB intersection and if its more than point light range, ignore the light since its not effective 
		object->getMaterial()->setPointLights(scene.getPointLights());
		object->render(viewMatrix, projection);
	}
	

	glPopDebugGroup();
}

void renderer_forward::setClearFlag(glm::vec4 color)
{
	m_ClearFlag = color;
}

void renderer_forward::init(modelViewer::render::object_factory& objectFactory)
{
	auto shaderLoader = objectFactory.getShaderLoader();
	auto vertShaderAsset = shaderLoader.load(m_DepthShaderVert, shaderType::vertex);
	auto fragShaderAsset = shaderLoader.load(m_DepthShaderFrag, shaderType::fragment);
	
	shader vertShader(vertShaderAsset);
	shader fragShader(fragShaderAsset);
	
	vertShader.compile();
	vertShader.verify();
	
	fragShader.compile();
	fragShader.verify();
	
	m_shadowProgram = std::make_unique<shader_program>(std::initializer_list<shader>{vertShader, fragShader});
    m_shadowProgram->validateLinking();
	
	m_shadowProgram->bind();
	m_MVPLocation = m_shadowProgram->getUniformLocation(m_MVPUniformName);

	m_shadowBuffer.bind();
	m_shadowBuffer.attachDepth(SHADOW_WIDTH, SHADOW_HEIGHT, true);
	m_shadowBuffer.unbind();

	auto textureLoader = objectFactory.getTextureLoader();
	auto textureAsset = textureLoader.load(m_EmptyShadowmapTexture,3);
	texture_setup setup;
	setup.assets.push_back(textureAsset);
	
	m_EmptyShadowmap = std::make_shared<texture_2D>(setup);


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
	skyboxTexture.type = texture_asset_type::cube;
	skyboxTexture.paths.emplace_back("res/textures/skybox/sample/right.jpg");
	skyboxTexture.paths.emplace_back("res/textures/skybox/sample/left.jpg");
    skyboxTexture.paths.emplace_back("res/textures/skybox/sample/top.jpg");
    skyboxTexture.paths.emplace_back("res/textures/skybox/sample/bottom.jpg");
    skyboxTexture.paths.emplace_back("res/textures/skybox/sample/front.jpg");
	skyboxTexture.paths.emplace_back("res/textures/skybox/sample/back.jpg");
    skyboxTexture.forceFlip = false;
	skyboxModel.material.textures.push_back(skyboxTexture);
	//skyboxModel.transform.setEularRotation(glm::vec3 (90,0,0));
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
