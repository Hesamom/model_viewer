﻿#include "mesh_renderer.h"
#include "object_renderer.h"

using namespace modelViewer::render;

mesh_renderer::mesh_renderer(std::shared_ptr <material>& material, std::shared_ptr <mesh>& mesh, common::transform& transform, std::string& name) : m_Transform{transform}
{
	m_Mesh = mesh;
	m_Material = material;
	m_Name = name;
	
	auto program = m_Material->getShaderProgram();
	m_Mesh->bindLayout(program);
	m_Material->bindProgram();
}

void mesh_renderer::render(glm::mat4 view, glm::mat4 projection)
{
	m_Material->bind();
	auto model =  m_Transform.getMatrix();
	auto modelView = view * model;
	m_Material->setModel(model);
	m_Material->setModelView(modelView);
	m_Material->setProjection(projection);

	m_Mesh->bind();

	switch (m_Material->getShaderProgram()->getTopology())
	{
		case topology_mode::triangles:
			m_Mesh->drawTriangles();
			break;
		case topology_mode::lines:
			m_Mesh->drawLines();
			break;
	}
}


void mesh_renderer::renderShadow() const {
	m_Mesh->bind();
	m_Mesh->drawTriangles();
}

std::shared_ptr<material> mesh_renderer::getMaterial()
{
	return m_Material;
}

bool mesh_renderer::getReceiveShadows() const
{
	return m_ReceivesShadow;
}

void mesh_renderer::setReceiveShadows(const bool enabled)
{
	m_ReceivesShadow = enabled;
}

const std::string& mesh_renderer::getName() const
{
	return m_Name;
}

bool mesh_renderer::getCastReflection() const
{
	return m_CastReflection;
}

void mesh_renderer::setCastReflection(bool enabled)
{
	m_CastReflection = enabled;
}

reflection_mode mesh_renderer::getReflectionMode() const
{
	return m_ReflectionMode;
}

void mesh_renderer::setReflectionMode(reflection_mode mode)
{
	m_ReflectionMode = mode;
}
