#include "object_renderer.h"

using namespace modelViewer::render;
using namespace modelViewer::common;

transform& object_renderer::getTransform() {
    return m_Transform;
}

void object_renderer::setTransform(const transform &t) {
    m_Transform = t;
}


void object_renderer::render(glm::mat4 view, glm::mat4 projection) {
	for (auto& renderer : m_MeshRenders) {
		renderer->render(view, projection);
	}
}


aabb calculateBoundingBox(std::vector<std::shared_ptr<mesh>>& meshes)
{
	glm::vec3 min{std::numeric_limits<float>::max()};
	glm::vec3 max{std::numeric_limits<float>::min()};
	
	for (auto& mesh : meshes)
	{
		auto box = mesh->getAsset()->getBoundingBox();
		
		auto boxMin = box.getMin();
		auto boxMax = box.getMax();
		
		min.x = std::min(boxMin.x, min.x);
		min.y = std::min(boxMin.y, min.y);
		min.z = std::min(boxMin.z, min.z);

		max.x = std::max(boxMax.x, max.x);
		max.y = std::max(boxMax.y, max.y);
		max.z = std::max(boxMax.z, max.z);
	}
	
	return {min, max};
}

object_renderer::object_renderer(std::vector<std::shared_ptr<material>>& materials,
	std::vector<std::shared_ptr<mesh>>& meshes, const std::string& name)
{
    m_Name = name;

	assert(materials.size() == meshes.size());

	for (int i = 0; i < meshes.size(); ++i) {
		std::string meshName = m_Name + "_" + std::to_string(i);
		meshes[i]->bindLayout(materials[i]->getShaderProgram());
		materials[i]->bindProgram();
		
		auto meshRender = std::make_shared<mesh_renderer>(materials[i], meshes[i], m_Transform, meshName);
		m_MeshRenders.push_back(meshRender);
	}

	setReceiveShadows(true);
	setCastReflection(true);
	setReflectionMode(reflection_mode::disabled);
    m_BaseBoundingBox = calculateBoundingBox(meshes);
}

//TODO remove duplication with the other ctor 
object_renderer::object_renderer(std::shared_ptr<material>& material,
	std::shared_ptr<mesh>& mesh_1,
	const std::string& name)
{
	m_Name = name;
	std::string meshName = m_Name + "_0";
	auto meshRender = std::make_shared<mesh_renderer>(material, mesh_1, m_Transform, meshName);
	m_MeshRenders.push_back(meshRender);

	std::vector<std::shared_ptr<mesh>> meshes;
	meshes.push_back(mesh_1);
	m_BaseBoundingBox = calculateBoundingBox(meshes);
}


void object_renderer::setCastShadow(bool enabled)
{
	m_CastShadows = enabled;
}

void object_renderer::setReceiveShadows(bool enabled)
{
	m_ReceiveShadows = enabled;

	for (auto& renderer : m_MeshRenders) {
		renderer->setReceiveShadows(m_ReceiveShadows);
	}
}

void object_renderer::computeBoundingBox() {
    if (!m_Transform.dirty) {
        return;
    }

    const auto transformation = m_Transform.getMatrix();
    const auto min = transformation * glm::vec4{m_BaseBoundingBox.getMin(),1};
    const auto max =  transformation * glm::vec4{m_BaseBoundingBox.getMax(),1};
    m_BoundingBox = aabb{min, max};
    
    m_Transform.dirty = false;
}

aabb object_renderer::getBoundingBox() {
    return m_BoundingBox;
}

bool object_renderer::getCastShadows()
{
	return  m_CastShadows;
}

bool object_renderer::getReceiveShadows()
{
	return  m_ReceiveShadows;
}

std::string object_renderer::getName() {
    return m_Name;
}

std::vector<std::shared_ptr<mesh_renderer>>& object_renderer::getRenderers()
{
	return m_MeshRenders;
}


bool object_renderer::getCastReflection() const
{
	return m_CastReflection;
}

void object_renderer::setCastReflection(bool enabled)
{
	m_CastReflection = enabled;
	for (auto& renderer : m_MeshRenders) {
		renderer->setCastReflection(enabled);
	}
}

reflection_mode object_renderer::getReflectionMode() const
{
	return m_ReflectionMode;
}

void object_renderer::setReflectionMode(reflection_mode mode)
{
	m_ReflectionMode = mode;
	for (auto& renderer : m_MeshRenders) {
		renderer->setReflectionMode(mode);
	}
}




