#include "material.h"


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
}

int material::getMaxSupportedTextureUnits() {
	static int maxTexturesFrag = 0;
	if (maxTexturesFrag > 0) {
		return maxTexturesFrag;
	}

	maxTexturesFrag = m_Device->getMaxSamplersPerProgram();
	return maxTexturesFrag;
}

std::shared_ptr<texture> material::getTextureForSampler(const shader_uniform_info& info, const std::vector<texture_binding>& textures) {

	for (const auto& texture : textures) {
		//current convention has ".mat" prefix 
		if (texture.samplerName == info.name || "u_mat." + texture.samplerName == info.name) {
			return texture.texture;
		}
	}

	
	shader_uniform_texture_pair pair = {info.type, info.textureUsage};
	if (m_DefaultTextures.contains(pair)) {
		return m_DefaultTextures[pair];
	}

	return nullptr;
}

void material::bindTextures(const std::vector<texture_binding>& textures)
{
	const auto activeUniforms = m_Program->getActiveUniforms();
	int textureUnit = 0;
	for (const auto & uniform : activeUniforms)
	{
		if (!uniform.isSampler())
			continue;
		
		if (uniform.isShadowSampler()) {
			continue;
		}
		
		auto texture = getTextureForSampler(uniform, textures);
		if (texture == nullptr) {
			std::cerr << "Failed to select a texture for uniform with name: " << uniform.name << "\n";
			continue;
		}

		m_Program->setUniform(uniform.name, textureUnit, true);
		m_ActiveTextures.push_back(texture);
		textureUnit++;
	}
}

void material::setCullingFaceMode(res::cull_face_mode mode)
{
	m_Program->setCullFaceMode(m_Info.propertySet.cullFaceMode);
}

void material::bind() {
    m_Program->bind();

	m_Device->setDepthmap(m_Info.propertySet.depthWriteEnabled);
	m_Program->setCullFaceMode(m_Info.propertySet.cullFaceMode);
	
    int index = 0;
    for (const auto &item: m_ActiveTextures)
    {
        item->active(index);
        index++;
    }
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

const std::vector<std::shared_ptr<texture>>& material::getBoundTextures() const
{
	return m_ActiveTextures;
}

std::shared_ptr<shader_program> material::getShaderProgram()
{
	return m_Program;
}
