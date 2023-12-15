#include "shader_program.h"
#include "material.h"
#include "texture_2D.h"


using namespace modelViewer::render;
using namespace modelViewer::res;


material::material(const material_info &info, std::vector<std::shared_ptr<texture>>& textures, std::shared_ptr<shader_program>& program) {

    m_Info = info;
    m_Program = program;

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

std::string material::getSamplerName(modelViewer::res::texture_asset_type type) {
    switch (type) {

        case texture_asset_type::none:
            break;
        case texture_asset_type::diffuse:
            return m_DiffuseSampler;
        case texture_asset_type::normal:
            return m_NormalSampler;
        case texture_asset_type::cube:
            return m_SkyboxSampler;
        default:
            throw std::runtime_error("not supported!");
    }
    return {};
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

void material::bindTextures(std::vector<std::shared_ptr<texture>>& textures)
{
    int index = 0;
	std::set<int> assignedLocs;
    for (const auto &item: textures)
    {
        auto loc = m_Program->getUniformLocation(getSamplerName(item->getType()));
        if (loc < 0)
        {
            continue;
        }
		if (assignedLocs.contains(loc))
		{
			throw std::runtime_error("the texture would override the previous ones since they share the same sampler");
		}

		assignedLocs.insert(loc);
        
        item->active(index);
        m_Program->bind();
		m_Program->setUniform(loc, index);
        index++;
        m_Textures.push_back(item);
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
    for (const auto &item: m_Textures)
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
	m_LightAmbientUniform.setValue(diffuse, *m_Program);
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

modelViewer::res::material_info& material::getInfo()
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
