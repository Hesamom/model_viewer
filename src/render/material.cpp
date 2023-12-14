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
	m_LightColorUniform.getLocation(*m_Program);
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

void material::setModelView(glm::mat4 &matrix) 
{
	m_ModelViewUniform.setValue(matrix, *m_Program);
}

void material::setModel(glm::mat4 &model)
{
	m_ModelUniform.setValue(model, *m_Program);
}

void material::setProjection(glm::mat4 &projection)
{
	m_ProjectionUniform.setValue(projection, *m_Program);
}

void material::applyMaterialProperties() {

	for (auto& floatProp : m_Info.propertySet.floats)
	{
		int loc = m_Program->getUniformLocation(floatProp.name);
		if (loc > -1)
		{
			m_Program->setUniformFloat(loc, floatProp.value);
		}
	}

	for (auto& intProp : m_Info.propertySet.ints)
	{
		int loc = m_Program->getUniformLocation(intProp.name);
		if (loc > -1)
		{
			m_Program->setUniformInt(loc, intProp.value);
		}
	}

	for (auto& colorProp : m_Info.propertySet.colors)
	{
		int loc = m_Program->getUniformLocation(colorProp.name);
		if (loc > -1)
		{
			m_Program->setUniformVector3(loc, colorProp.value);
		}
	}


	for (auto& boolProp : m_Info.propertySet.booleans)
	{
		int loc = m_Program->getUniformLocation(boolProp.name);
		if (loc > -1)
		{
			m_Program->setUniformInt(loc, boolProp.value);
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
        m_Program->setUniformInt(loc, index);
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
	
	auto pos = light.getPosition();
	m_LightPosUniform.setValue(pos, *m_Program);

	auto color = light.getColor();
    m_LightColorUniform.setValue(color, *m_Program);
}


void material::setShadowMapSlot(int slot)
{
	if (m_ShadowMapSamplerLocation < 0)
	{
		return;
	}

	m_Program->bind();
	m_Program->setUniformInt(m_ShadowMapSamplerLocation, slot);
}

void material::setLightViewProjection(glm::mat4& matrix)
{
	m_LightViewProjectionUniform.setValue(matrix, *m_Program);
}

modelViewer::res::material_info& material::getInfo()
{
	return m_Info;
}
