﻿#include "material.h"


using namespace modelViewer::render;
using namespace modelViewer::res;


material::material(std::shared_ptr<gfx_device>& device, const res::material_asset& info, std::vector<texture_binding>& textures, std::shared_ptr<shader_program>& program, std::map<shader_uniform_texture_pair, std::shared_ptr<texture>>& defaultTextures) {

	m_Device = device;
    m_Info = info;
    m_Program = program;
	m_DefaultTextures = defaultTextures;

    m_Program->bind();
	
	setShadowMapSlot(getMaxSupportedTextureUnits());
	setSpotShadowMapSlot(getMaxSupportedTextureUnits() - 2);
	setReflectionMapSlot(getMaxSupportedTextureUnits() - 3);
	
    applyMaterialProperties();
    bindTextures(textures);
}



void material::setMVP(glm::mat4 &matrix) 
{
    m_MVPUniform.setValue(matrix, *m_Program, true);
}

void material::setModelView( glm::mat4 &matrix) 
{
	m_ModelViewUniform.setValue(matrix, *m_Program, true);
}

void material::setModel( glm::mat4 &model)
{
	m_ModelUniform.setValue(model, *m_Program, true);
}

void material::setProjection( glm::mat4 &projection)
{
	m_ProjectionUniform.setValue(projection, *m_Program, true);
}

void material::applyMaterialProperties() {

	for (auto& floatProp : m_Info.propertySet.floats)
	{
		m_Program->setUniform(floatProp.name, floatProp.value, true);
	}

	for (auto& intProp : m_Info.propertySet.ints)
	{
		m_Program->setUniform(intProp.name, intProp.value, true);
	}

	for (auto& colorProp : m_Info.propertySet.colors)
	{
		m_Program->setUniform(colorProp.name, colorProp.value, true);
	}
	
	for (auto& boolProp : m_Info.propertySet.booleans)
	{
		m_Program->setUniform(boolProp.name, boolProp.value, true);
	}

	m_Program->setDepthMap(m_Info.propertySet.depthWriteEnabled);
	m_Program->setCullFaceMode(m_Info.propertySet.cullFaceMode);
}

int material::getMaxSupportedTextureUnits() {
	static int maxTexturesFrag = 0;
	if (maxTexturesFrag > 0) {
		return maxTexturesFrag;
	}

	maxTexturesFrag = m_Device->getMaxSamplersPerProgram();
	return maxTexturesFrag;
}

std::shared_ptr<texture> material::getTextureForSampler(const shader_texture_slot& slot, const std::vector<texture_binding>& bindings) {

	for (const auto& binding : bindings) 
	{
		if (binding.samplerName == slot.name && binding.texture->getType() == slot.type)
		{
			return binding.texture;
		}
	}

	//TODO fix this later 
	shader_uniform_texture_pair pair = {slot.type, getUsageByName(slot.name)};
	if (m_DefaultTextures.contains(pair))
	{
		return m_DefaultTextures[pair];
	}

	return nullptr;
}

shader_texture_usage material::getUsageByName(const std::string& textureName)
{
	if (textureName.ends_with("_diffuse")) {
		return shader_texture_usage::diffuse;
	}
	if (textureName.ends_with("_normal")) {
		return shader_texture_usage::normal;
	}
	if (textureName.ends_with("_sec")) {
		return shader_texture_usage::specular;
	}
	
	return shader_texture_usage::none;
}


void material::bindTextures(const std::vector<texture_binding>& textures)
{
	const auto textureSlots = m_Program->getTextureSlots();
	int textureUnit = 0;
	for (const auto & slot : textureSlots)
	{
		auto texture = getTextureForSampler(slot, textures);
		if (texture == nullptr)
		{
			std::cerr << "Failed to select a texture for slot with name: " << slot.name << "\n";
			continue;
		}
		
		m_Program->bindTexture(textureUnit, texture);
		textureUnit++;
	}
}

void material::setCullingFaceMode(res::cull_face_mode mode)
{
	m_Program->setCullFaceMode(m_Info.propertySet.cullFaceMode);
}

void material::bind() {
	
    m_Program->bind();
}


void material::setCameraPosition(glm::vec3 position) {
	 m_CameraPositionUniform.setValue(position, *m_Program, true);
}

void material::setDirectionalLight(const light_directional &light) {
    
    m_Program->bind();
	
	auto direction = light.direction;
	m_LightDirUniform.setValue(direction, *m_Program, true);
	auto ambient = light.ambient;
    m_LightAmbientUniform.setValue(ambient, *m_Program, true);
	auto diffuse = light.diffuse;
	m_LightDiffuseUniform.setValue(diffuse, *m_Program, true);
}


void material::setShadowMapSlot(int slot) const {
	
	m_Program->bind();
	m_Program->setUniform(m_ShadowSampler, slot, true);
}

void material::setReflectionMapSlot(int slot) const {

	m_Program->bind();
	m_Program->setUniform(m_ReflectionSampler, slot, true);
}

void material::setSpotShadowMapSlot(int slot) const {
	
	m_Program->bind();
	m_Program->setUniform(m_SpotShadowSampler, slot, true);
}

void material::setLightViewProjection(glm::mat4& matrix)
{
	m_LightViewProjectionUniform.setValue(matrix, *m_Program, true);
}

material_asset& material::getInfo()
{
	return m_Info;
}

void material::setSpotLights(std::vector<light_spot>& lights) {
	int index = 0;
	const std::string blockName = "u_spotLights";
	m_Program->setUniform("u_spotLightCount", (int)lights.size(), true);
	
	for (auto& item : lights)
	{
		shader_uniform<glm::mat4x4> viewTransform{"m_SpotLightViewProjection","", index};
		viewTransform.setValue(item.viewProjection, *m_Program, true);

		shader_uniform<glm::vec3> positionMember{"position",blockName, index};
		positionMember.setValue(item.position, *m_Program, true);

		shader_uniform<glm::vec3> directionMember{"direction",blockName, index};
		directionMember.setValue(item.direction, *m_Program, true);

		shader_uniform<glm::vec3> ambientMember{"ambient",blockName, index};
		ambientMember.setValue(item.ambient, *m_Program, true);

		shader_uniform<glm::vec3> diffuseMember{"diffuse",blockName, index};
		diffuseMember.setValue(item.diffuse, *m_Program, true);

		
		shader_uniform<float> rangeMember{"innerCutoff",blockName, index};
		auto innerCutoff = cos(glm::radians(item.innerCutoff));
		rangeMember.setValue(innerCutoff, *m_Program, true);

		shader_uniform<float> insideRangeMember{"outerCutoff",blockName, index};
		auto outterCutoff = cos(glm::radians(item.outerCutoff));
		insideRangeMember.setValue(outterCutoff, *m_Program, true);
		
		index++;
	}
}

void material::setPointLights(std::vector<light_point>& lights)
{
	//TODO can cache the uniforms here 
	int index = 0;
	const std::string blockName = "u_pointLights";
	m_Program->setUniform("u_pointLightCount", (int)lights.size(), true);
	
	for (auto& item : lights)
	{
		shader_uniform<glm::vec3> positionMember{"position",blockName, index};
		positionMember.setValue(item.position, *m_Program, true);

		shader_uniform<glm::vec3> ambientMember{"ambient",blockName, index};
		ambientMember.setValue(item.ambient, *m_Program, true);

		shader_uniform<glm::vec3> diffuseMember{"diffuse",blockName, index};
		diffuseMember.setValue(item.diffuse, *m_Program, true);

		float range = 0;
		float insideRange = 0;
		item.getRanges(range, insideRange);
		shader_uniform<float> rangeMember{"range",blockName, index};
		rangeMember.setValue(range, *m_Program, true);

		shader_uniform<float> insideRangeMember{"insideRange",blockName, index};
		insideRangeMember.setValue(insideRange, *m_Program, true);
		
		index++;
	}
}

bool material::isReflective() const
{
	return m_Program->hasUniform(m_ReflectionSampler);
}


std::shared_ptr<shader_program> material::getShaderProgram()
{
	return m_Program;
}


