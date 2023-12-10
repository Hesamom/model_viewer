
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include "camera.h"

glm::vec3 modelViewer::render::camera::getPosition() const
{
	return m_Position;
}

void modelViewer::render::camera::setPosition(glm::vec3& position)
{
	m_Position = position;
}

glm::mat4 modelViewer::render::camera::getView() const
{
	auto viewMatrix = glm::lookAt(
		m_Position,
		glm::vec3(0), 
		glm::vec3(0,1,0));
		
	return viewMatrix;
}

glm::mat4 modelViewer::render::camera::getProjection() const
{
	auto aspectRatio = (float)m_ViewportWidth/(float)m_ViewportHeight;
	float fov = m_Frustum.getFieldOfView();
	float near, far;
	m_Frustum.getPlanes(near, far);
	auto projection = glm::perspective<float>(fov,aspectRatio,near,far);
	
	return projection;
}

frustum_perspective& modelViewer::render::camera::getFrustum()
{
	return m_Frustum;
}

void modelViewer::render::camera::getViewport(int& width, int& height) const
{
	width = m_ViewportWidth;
	height = m_ViewportHeight;
}

void modelViewer::render::camera::setViewPort(int width, int height)
{
	if (width <= 0 || height <= 0) 
	{
		throw std::invalid_argument("Viewport dimensions should be greater than 0");
	}
	
	m_ViewportWidth = width;
	m_ViewportHeight = height;
}
