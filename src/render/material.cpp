﻿#include "shader_program.h"
#include "material.h"
#include "texture_2D.h"


using namespace modelViewer::render;
using namespace modelViewer::res;


material::material(const material_info& info, std::vector<texture_binding>& textures, std::shared_ptr<shader_program>& program, std::unordered_map<shader_uniform_type, std::shared_ptr<texture>>& defaultTextures) {

    m_Info = info;
    m_Program = program;
	m_DefaultTetxures = defaultTextures;

    m_Program->bind();
	m_ModelUniform.getLocation(*m_Program);
    m_ModelViewUniform.getLocation(*m_Program);
	m_MVPUniform.getLocation(*m_Program);
	m_ProjectionUniform.getLocation(*m_Program);
	
	m_LightViewProjectionUniform.getLocation(*m_Program);
	m_LightAmbientUniform.getLocation(*m_Program);
	m_LightDiffuseUniform.getLocation(*m_Program);
	m_LightPosUniform.getLocation(*m_Program);
	
	m_ShadowMapSamplerLocation = m_Program->getUniformLocation(m_ShadowSampler);
    
    applyMaterialProperties();
    bindTextures(textures);
}


void material::setMVP(glm::mat4 &matrix) 
{
    m_MVPUniform.setValue(matrix, *m_Program);
}

void material::setModelView( glm::mat4 &matrix) 
{
	m_ModelViewUniform.setValue(matrix, *m_Program);
}

void material::setModel( glm::mat4 &model)
{
	m_ModelUniform.setValue(model, *m_Program);
}

void material::setProjection( glm::mat4 &projection)
{
	m_ProjectionUniform.setValue(projection, *m_Program);
}

void material::applyMaterialProperties() {

	for (auto& floatProp : m_Info.propertySet.floats)
	{
		int loc = m_Program->getUniformLocation(floatProp.name);
		if (loc > -1)
		{
			m_Program->setUniform(loc, floatProp.value);
		}
	}

	for (auto& intProp : m_Info.propertySet.ints)
	{
		int loc = m_Program->getUniformLocation(intProp.name);
		if (loc > -1)
		{
			m_Program->setUniform(loc, intProp.value);
		}
	}

	for (auto& colorProp : m_Info.propertySet.colors)
	{
		int loc = m_Program->getUniformLocation(colorProp.name);
		if (loc > -1)
		{
			m_Program->setUniform(loc, colorProp.value);
		}
	}


	for (auto& boolProp : m_Info.propertySet.booleans)
	{
		int loc = m_Program->getUniformLocation(boolProp.name);
		if (loc > -1)
		{
			m_Program->setUniform(loc, boolProp.value);
		}
	}
}

std::shared_ptr<texture> material::getTextureForSampler(const std::string& samplerName, shader_uniform_type type, const std::vector<texture_binding>& textures) {

	for (const auto& texture : textures) {
		if (texture.samplerName == samplerName) {
			return texture.texture;
		}
	}

	if (m_DefaultTetxures.contains(type)) {
		return m_DefaultTetxures[type];
	}

	return nullptr;
}

void material::bindTextures(const std::vector<texture_binding>& textures)
{
	const auto activeUniforms = m_Program->getActiveUniforms();
	int textureUnit = 0;
	for (const auto & uniform : activeUniforms)
	{
		if (uniform.isSampler())
		{
			if (uniform.isShadowSampler()) {
				continue;
			}
			
			auto texture = getTextureForSampler(uniform.name, uniform.type, textures);
			if (texture == nullptr) {
				std::cerr << "Failed to select a texture for uniform with name: " << uniform.name << "\n";
				continue;
			}

			const auto loc = m_Program->getUniformLocation(uniform.name);
			m_Program->setUniform(loc, textureUnit);
			m_ActiveTextures.push_back(texture);
			textureUnit++;
		}
	}
}

void material::bind() {
    m_Program->bind();
	
	glDepthMask(m_Info.propertySet.depthWriteEnabled);
	if (m_Info.propertySet.cullFaceEnabled)
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}
	
    int index = 0;
    for (const auto &item: m_ActiveTextures)
    {
        item->active(index);
        index++;
    }
}

int material::getUniformLocation(std::string name) const {
    return m_Program->getUniformLocation(name);
}

int material::getAttributeLocation(std::string name) const {
    return m_Program->getAttributeLocation(name);
}

void material::setLight(const light_directional &light) {
    
    m_Program->bind();
	
	auto pos = light.position;
	m_LightPosUniform.setValue(pos, *m_Program);
	auto ambient = light.ambient;
    m_LightAmbientUniform.setValue(ambient, *m_Program);
	auto diffuse = light.diffuse;
	m_LightDiffuseUniform.setValue(diffuse, *m_Program);
}


void material::setShadowMapSlot(int slot)
{
	if (m_ShadowMapSamplerLocation < 0)
	{
		return;
	}

	m_Program->bind();
	m_Program->setUniform(m_ShadowMapSamplerLocation, slot);
}

void material::setLightViewProjection(glm::mat4& matrix)
{
	m_LightViewProjectionUniform.setValue(matrix, *m_Program);
}

material_info& material::getInfo()
{
	return m_Info;
}

void material::setPointLights(std::vector<light_point>& lights)
{
	//TODO can cache the uniforms here 
	int index = 0;
	const std::string blockName = "pointLights";
	m_Program->setUniform(m_Program->getUniformLocation("u_pointLightCount"), (int)lights.size());
	
	for (auto& item : lights)
	{
		shader_uniform<glm::vec3> positionMember{"position",blockName, index};
		positionMember.getLocation(*m_Program);
		positionMember.setValue(item.position, *m_Program);

		shader_uniform<glm::vec3> ambientMember{"ambient",blockName, index};
		ambientMember.getLocation(*m_Program);
		ambientMember.setValue(item.ambient, *m_Program);

		shader_uniform<glm::vec3> diffuseMember{"diffuse",blockName, index};
		diffuseMember.getLocation(*m_Program);
		diffuseMember.setValue(item.diffuse, *m_Program);

		float range = 0;
		float insideRange = 0;
		item.getRanges(range, insideRange);
		shader_uniform<float> rangeMember{"range",blockName, index};
		rangeMember.getLocation(*m_Program);
		rangeMember.setValue(range, *m_Program);

		shader_uniform<float> insideRangeMember{"insideRange",blockName, index};
		insideRangeMember.getLocation(*m_Program);
		insideRangeMember.setValue(insideRange, *m_Program);
		
		index++;
	}
}
