
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include "renderer_forward.h"
#include "camera.h"

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

	float near_plane = 1.0f, far_plane = 100.0f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	glm::mat4 lightView = glm::lookAt(scene.getLight().getPosition(),
		glm::vec3( 0.0f, 0.0f,  0.0f),
		glm::vec3( 0.0f, 1.0f,  0.0f));
	m_LightViewProjection = lightProjection * lightView;
	
	m_shadowProgram->bind();
	for (auto& object : scene.getObjects()) {

		auto mvp = m_LightViewProjection * object->getTransform().getMatrix();
		m_shadowProgram->setUniformMatrix4(m_MVPLocation, mvp);
		object->renderShadow();
	}
	
	m_shadowBuffer.unbind();
	
	glPopDebugGroup();
}

void renderer_forward::renderObjects(render_scene& scene, camera& camera, bool shadowsEnabled)
{
	int viewportWidth, viewportHeight;
	camera.getViewport(viewportWidth, viewportHeight);
	glViewport(0, 0, viewportWidth, viewportHeight);
	
	glClearBufferfv(GL_COLOR, 0, &m_ClearFlag.x);
	glClear(GL_DEPTH_BUFFER_BIT);
	

	auto viewMatrix = camera.getView();
	auto projection = camera.getProjection();

	const int reservedShadowMapSlot = 32;
	
	if(shadowsEnabled)
	{
		//TODO consider using another approach that would not be overriden by material, this is safe by now since we dont have 31 textures yet
		m_shadowBuffer.activateDepthMap(reservedShadowMapSlot);
	}
	
	
	//m_Platform.draw(viewMatrix, projection);
	for (auto& object : scene.getObjects()) {
		
		if(shadowsEnabled)
		{
			object->getMaterial()->setShadowMapSlot(reservedShadowMapSlot);
			object->getMaterial()->setLightViewProjection(m_LightViewProjection);
		}
		
		object->setLight(scene.getLight());
		object->render(viewMatrix, projection, render_mode::triangles);
	}
}

void renderer_forward::setClearFlag(glm::vec4 color)
{
	m_ClearFlag = color;
}

void renderer_forward::init(modelViewer::res::shader_loader shaderLoader)
{
	auto vertShaderAsset = shaderLoader.load(m_DepthShaderVert, shaderType::vertex);
	auto fragShaderAsset = shaderLoader.load(m_DepthShaderFrag, shaderType::fragment);
	
	shader vertShader(vertShaderAsset);
	shader fragShader(fragShaderAsset);
	
	vertShader.compile();
	vertShader.verify();
	
	fragShader.compile();
	fragShader.verify();
	
	m_shadowProgram = std::make_unique<shader_program>(std::initializer_list<shader>{vertShader, fragShader});
	m_shadowProgram->validate();
	
	m_shadowProgram->bind();
	m_MVPLocation = m_shadowProgram->getUniformLocation(m_MVPUniformName);

	m_shadowBuffer.bind();
	m_shadowBuffer.attachDepth(SHADOW_WIDTH, SHADOW_HEIGHT);
	m_shadowBuffer.unbind();
}
